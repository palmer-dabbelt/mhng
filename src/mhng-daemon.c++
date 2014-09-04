
/*
 * Copyright (C) 2014 Palmer Dabbelt
 *   <palmer@dabbelt.com>
 *
 * This file is part of mhng.
 *
 * mhng is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mhng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with mhng.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <libmhng/daemon/message.h++>
#include <libmhng/daemon/process.h++>
#include <libmhng/args.h++>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Creates a new server-side SEQPACKET, UNIX-domain socket. */
static int create_socket(const std::string path);

/* This thread fires on a new client connection.  It blocks a whole
 * bunch handling connections from the client  */
static void client_main(int client);

/* Waits for someone to request a synchronization, forks off a
 * synchronization process, and if that is successful updates everyone
 * waiting for a synchronization. */
static void sync_main(void);

/* Keeps running the IDLE process forever. */
static void idle_main(void);

/* Here we have two counters: one that contains the latest requested
 * synchronization, and another that contains the latest responded
 * synchronization.  Essentially the idea here is that client threads
 * will increment a request for synchronization whenever they're
 * provided with a sync command, signal, and then sleep.  The special
 * sync thread will  */
static std::atomic<long> sync_req;
static std::atomic<long> sync_rep;
static std::mutex sync_lock;
static std::condition_variable sync_signal;

/* Here is a little health bit for the network -- essentially what
 * happens here is that sometimes the network can go down, which means
 * that we don't want to continue to check for mail.  Note that this
 * lock is locked by the sync lock! */
static std::atomic<bool> net_up;

/* These two processes are global so everyone can just go ahead and
 * kill them whenever they want! */
static mhng::daemon::process sync_process(
        __PCONFIGURE__PREFIX "/libexec/mhng/mhimap-sync",
        "mhimap-sync"
    );
static mhng::daemon::process idle_process(
        __PCONFIGURE__PREFIX "/libexec/mhng/mhimap-idle",
        "mhimap-idle"
    );

int main(int argc, const char **argv)
{
    sync_req = 0;
    sync_rep = 0;
    net_up = true;

    auto args = mhng::args::parse_all_folders(argc, argv);

    /* This special thread responds to synchronization requests. */
    {
        std::thread sync_thread(sync_main);
        sync_thread.detach();
    }

    /* Begin listening for client connections from other programs
     * running on this system. */
    int server = create_socket(args->mbox()->path() + "/daemon.socket");

    /* Fires up the IDLE thread, which needs that server connection to
     * have been created so it can send us messages. */
    {
        std::thread idle_thread(idle_main);
        idle_thread.detach();
    }

    /* Accepts every client connection forever! */
    while (true) {
        struct sockaddr_un addr;
        socklen_t len = sizeof(addr);
        int client = accept(server, (struct sockaddr *)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            abort();
        }

        std::thread client_thread(client_main, client);
        client_thread.detach();
    }
    close(server);

    return 0;
}

int create_socket(const std::string path)
{
    int server = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (server < 0) {
        perror("Unable to create UNIX socket\n");
        abort();
    }

    struct sockaddr_un local;
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, path.c_str());
    unlink(local.sun_path);
    socklen_t len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(server, (struct sockaddr*)(&local), len) < 0) {
        perror("Unable to bind\n");
        abort();
    }

    if (listen(server, 5) < 0) {
        perror("Unable to listen");
        abort();
    }

    return server;
}

void client_main(int client)
{
    while (true) {
        char buf[BUFFER_SIZE];
        auto rs = recv(client, buf, BUFFER_SIZE, 0);
        if (rs < 0) {
            perror("Error while receiving");
            close(client);
            return;
        }

        if (rs == 0) {
            close(client);
            return;
        }

        auto msg = mhng::daemon::message::deserialize(buf, rs);
        switch (msg->type()) {
        case mhng::daemon::message_type::SYNC:
        {
            /* Trigger the sync thread to go to the server and
             * actually do something. */
            std::unique_lock<std::mutex> lock(sync_lock);
            auto ticket = ++sync_req;
            sync_signal.notify_all();
            sync_signal.wait(lock, [&]{ return ticket <= sync_rep; });

            break;
        }

        case mhng::daemon::message_type::NET_UP:
        {
            fprintf(stderr, "Enabling Networking\n");

            std::unique_lock<std::mutex> lock(sync_lock);

            net_up = true;

            idle_process.kill();
            sync_process.kill();

            sync_signal.notify_all();

            break;
        }

        case mhng::daemon::message_type::NET_DOWN:
        {
            fprintf(stderr, "Disabling Networking\n");

            std::unique_lock<std::mutex> lock(sync_lock);

            net_up = false;

            idle_process.kill();
            sync_process.kill();

            sync_signal.notify_all();

            break;
        }

        case mhng::daemon::message_type::RESPONSE:
        {
            fprintf(stderr, "Unable to handle RESPONSE packet\n");
            fprintf(stderr, "  mhng-daemon shouldn't get these\n");
            abort();
            break;
        }

        }

        auto resp = msg->response();
        auto len = mhng::daemon::message::serialize(msg, buf, BUFFER_SIZE);
        auto ss = send(client, buf, len, 0);
        if (ss < 0) {
            if (errno != EPIPE)
                perror("Error while sending");
            close(client);
            return;
        }
    }

    close(client);
}

void sync_main(void)
{
    while (true) {
        /* The first thing to do is to atomicly wait for someone to
         * request a synchronization while obtaining a response
         * ticket. */
        auto get_ticket_and_fork = [&](void) -> long
            {
                /* First wait for someone to request a synchronization. */
                std::unique_lock<std::mutex> lock(sync_lock);
                sync_signal.wait(
                    lock,
                    [&]{
                        /* If the network is down then it doesn't
                         * matter if a sync has been requested, we
                         * can't do that anyway. */
                        if (net_up == false)
                            return false;

                        /* If nobody has requested a sync then don't
                         * proceed. */
                        if (sync_rep == sync_req)
                            return false;

                        return true;
                    });

                /* We need to fork with the lock held because if we
                 * don't then there's a race condition later killing
                 * the process. */
                sync_process.fork();

                /* Our ticket is just the ticket that the latest
                 * requester gave us.  This means that every request
                 * before we start will be satisfied! */
                return sync_req;
            };

        auto ticket = get_ticket_and_fork();
        int status = sync_process.join();

        /* If the synchronization didn't succeed then for now just
         * pretend it didn't happen at all and instead just go ahead
         * and try again.  Note that there's a simple rate limit in
         * here... */
        if (status != 0) {
            fprintf(stderr, "Synchronization failed, retrying\n");
            sleep(5);
            continue;
        }

        /* Now that we've actually synchronized it's time to go ahead
         * and inform all the clients that we've done so. */
        std::unique_lock<std::mutex> lock(sync_lock);
        if (sync_rep >= ticket) {
            fprintf(stderr, "Race condition!\n");
            abort();
        }
        sync_rep = ticket;
        sync_signal.notify_all();
    }
}

void idle_main(void)
{
    while (true) {
        /* We only want to bother trying to idle when we already know
         * the network is up.  This waits for the network to go up
         * before continuing. */
        {
            std::unique_lock<std::mutex> lock(sync_lock);
            sync_signal.wait(
                lock,
                [&]{
                    if (net_up == false)
                        return false;

                    return true;
                });

            idle_process.fork();
        }

        int status = idle_process.join();

        if (status != 0) {
            fprintf(stderr, "IDLE failed, retrying\n");
            sleep(5);
            continue;
        }
    }
}
