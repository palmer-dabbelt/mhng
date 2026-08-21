/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <atomic>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <list>
#include <libmhng/daemon/message.h++>
#include <libmhng/daemon/process.h++>
#include <libmhng/args.h++>
#include <libmhng/logfile.h++>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Creates a new server-side SEQPACKET, UNIX-domain socket. */
static int create_socket(const std::string path);

/* This thread fires on a new client connection.  It blocks a whole
 * bunch handling connections from the client  */
static void client_main(int client);

/* Returns true if the client on the far end of this socket has gone
 * away (orderly shutdown or error), peeking without consuming data.  A
 * client_main thread that is blocked waiting for a sync to progress uses
 * this to notice a disconnected peer instead of holding the fd (and the
 * thread) open forever -- the leak that used to wedge the whole daemon
 * once ~250 waiters piled up and RLIMIT_NOFILE was hit. */
static bool client_gone(int client);

/* How long a waiting client_main thread sleeps between re-checking both
 * its wakeup condition and whether the peer is still connected. */
static const std::chrono::seconds client_wait_poll(5);

/* Waits for someone to request a synchronization, forks off a
 * synchronization process, and if that is successful updates everyone
 * waiting for a synchronization. */
static void sync_main(mhng::daemon::process* idle);

/* Keeps running the IDLE process forever. */
static void idle_main(mhng::daemon::process &idle, std::string account_name);

/* Keeps running the OAUTH2 process forever. */
static void oauth2_main(mhng::daemon::process* oauth2);

/* Here we have two counters: one that contains the latest requested
 * synchronization, and another that contains the latest responded
 * synchronization.  Essentially the idea here is that client threads
 * will increment a request for synchronization whenever they're
 * provided with a sync command, signal, and then sleep.  The special
 * sync thread will  */
static std::atomic<long> sync_req;
static std::atomic<long> sync_rep;
static std::atomic<uint64_t> sync_largest_uid;
static std::mutex sync_lock;
static std::condition_variable sync_signal;

/* Here is a little health bit for the network -- essentially what
 * happens here is that sometimes the network can go down, which means
 * that we don't want to continue to check for mail.  Note that this
 * lock is locked by the sync lock! */
static std::atomic<bool> net_up;

/* A counter of any folder event that happens. */
static std::atomic<uint32_t> event_ticket;
static std::mutex event_lock;
static std::condition_variable event_signal;

/* These two process types are global so everyone can just go ahead and
 * kill them whenever they want! */
static std::list<mhng::daemon::process> sync_processes;
static std::list<mhng::daemon::process> idle_processes;
static std::list<mhng::daemon::process> oauth2_processes;

/* A global context */
static std::shared_ptr<mhng::args> args = NULL;
static std::mutex args_lock;

/* Failures come in runs: a server that just dropped us will usually
 * drop us again, and a CA bundle that's gone missing isn't going to
 * come back on its own.  Retrying every five seconds forever just
 * fills the log and hammers the server, so back the retries off. */
static const int retry_delay_min = 5;
static const int retry_delay_max = 60;

/* Waits out the current backoff and then doubles it, but gives up on
 * the wait as soon as the network changes state: there's no reason to
 * sit out the rest of a minute earned by a dead network when the lid
 * has just been opened. */
static void wait_before_retry(int &delay)
{
    bool was_up = net_up;

    for (int i = 0; i < delay; ++i) {
        sleep(1);

        if (net_up != was_up) {
            delay = retry_delay_min;
            return;
        }
    }

    delay *= 2;
    if (delay > retry_delay_max)
        delay = retry_delay_max;
}

int main(int argc, const char **argv)
{
    sync_req = 0;
    sync_rep = 0;
    net_up = true;

    /* The daemon holds one fd (and one thread) per in-flight client
     * request, and requests that block waiting for a slow or stalled
     * sync can accumulate.  launchd's default soft limit of 256 open
     * files is low enough that a backlog can exhaust it, at which point
     * even opening the TLS CA file fails ("Error while reading file")
     * and every login wedges permanently.  Raise the soft limit toward
     * the hard limit so a backlog degrades gracefully.  (client_gone()
     * keeps the backlog from being a true leak; this is headroom.) */
    {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
            rlim_t want = 4096;
            rlim_t cap = (rl.rlim_max == RLIM_INFINITY || rl.rlim_max > want)
                         ? want : rl.rlim_max;
            if (rl.rlim_cur < cap) {
                rl.rlim_cur = cap;
                if (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                    perror("setrlimit(RLIMIT_NOFILE)");
            }
        } else {
            perror("getrlimit(RLIMIT_NOFILE)");
        }
    }

#ifdef __APPLE__
    {
        struct sigaction sa;
        sa.sa_handler = SIG_IGN;
        sa.sa_flags = 0;
        if (sigaction(SIGPIPE, &sa, nullptr) < 0) {
            perror("sigaction");
        }
    }
#endif

    bool verbose_imap = false;
    bool allow_without_launchctl = false;
    std::vector<const char *> fargv;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--verbose-imap") == 0)
            verbose_imap = true;
        else if (strcmp(argv[i], "--allow-running-without-launchctl") == 0)
            allow_without_launchctl = true;
        else
            fargv.push_back(argv[i]);
    }

#ifdef __APPLE__
    /* When launchd spawns us as a LaunchAgent it sets XPC_SERVICE_NAME to
     * the job label; a shell or detached session leaves it unset or "0".
     * Outside launchd the process inherits a bootstrap port that can't
     * reach mDNSResponder, so getaddrinfo() fails forever ("nodename nor
     * servname provided") in every IMAP child we fork()+exec() -- even
     * while the network is up.  Bail out early with a clear message rather
     * than spin the retry loop, unless the operator has opted in.  This
     * runs before redirect_log() so the message reaches the terminal. */
    if (!allow_without_launchctl) {
        const char *xpc = getenv("XPC_SERVICE_NAME");
        bool launchd_managed = (xpc != NULL) && (strcmp(xpc, "0") != 0);
        if (!launchd_managed) {
            fprintf(stderr,
                "mhng-daemon: refusing to start outside launchd on macOS.\n"
                "  A process started from a shell or detached session inherits a\n"
                "  bootstrap port that cannot reach mDNSResponder, so getaddrinfo()\n"
                "  fails forever in every IMAP child even while the network is up.\n"
                "  Start it via its LaunchAgent instead:\n"
                "    launchctl kickstart -k gui/$(id -u)/com.dabbelt.mhng.daemon\n"
                "  Pass --allow-running-without-launchctl to override this check.\n");
            exit(1);
        }
    }
#endif

    args = mhng::args::parse_all_folders(fargv.size(), fargv.data());

    /* Own our own log rather than relying on a shell redirect or a
     * LaunchAgent StandardErrorPath: redirect stdout/stderr into
     * ~/.mhng/daemon.log and rotate it by size so it can't grow forever.
     * This has to happen before we log anything, and after arg parsing so
     * we know where the mailbox (and hence the log) lives.  The IMAP/OAUTH
     * children we fork()+exec() below inherit this redirect for free. */
    auto log = mhng::redirect_log(args->mbox()->path() + "/daemon.log");
    log->start_rotation_thread(60);

    std::cerr << "\n\n\n\n\n\nMHng Daemon Starting" << std::endl;

    auto imap_args = [&](std::string cmd, std::string account) {
        std::vector<std::string> a = {cmd, account};
        if (verbose_imap)
            a.push_back("--verbose");
        return a;
    };

    fprintf(stderr, "starting\n");
    for (const auto& account: args->mbox()->accounts()) {
        fprintf(stderr, "account->name %s\n", account->name().c_str());
        sync_processes.emplace_back(
            __PCONFIGURE__PREFIX "/libexec/mhng/mhimap-sync",
            imap_args("mhimap-sync", account->name())
        );
    }

    for (const auto& account: args->mbox()->accounts()) {
        if (!account->is_oauth2())
            continue;

        oauth2_processes.emplace_back(
            __PCONFIGURE__PREFIX "/libexec/mhng/mhoauth-refresh_loop",
            std::vector<std::string>({"mhoauth-refresh_loop", account->name()})
        );
    }

    /* This special thread responds to synchronization requests. */
    for (auto& sync_process: sync_processes) {
        std::thread sync_thread(sync_main, &sync_process);
        sync_thread.detach();
    }

    /* Begin listening for client connections from other programs
     * running on this system. */
    auto server = []() {
        std::unique_lock<std::mutex> lock(args_lock);
        return create_socket(args->mbox()->path() + "/daemon.socket");
    }();

    /* Fires up the IDLE thread, which needs that server connection to
     * have been created so it can send us messages. */
    for (const auto& account: args->mbox()->accounts()) {
        auto& idle_process = idle_processes.emplace_back(
            __PCONFIGURE__PREFIX "/libexec/mhng/mhimap-idle",
            imap_args("mhimap-idle", account->name()),
            10 * 60
        );

        std::thread idle_thread(
            [account, &idle_process] {
                idle_main(idle_process, account->name());
            }
        );
        idle_thread.detach();
    }

    /* Launches threads to babysit the OAUTH2 processes. */
    for (auto& oauth2_process: oauth2_processes) {
        std::thread oauth2_thread(oauth2_main, &oauth2_process);
        oauth2_thread.detach();
    }

    /* Accepts every client connection forever! */
    while (true) {
        struct sockaddr_un addr;
        socklen_t len = sizeof(addr);
        int client = accept(server, (struct sockaddr *)&addr, &len);

        if (client < 0) {
            auto old_errno = errno;
            perror("Unable to accept");

            if (old_errno == EINTR)
                continue;

            abort();
        }

#ifdef __APPLE__
        {
            /* http://stackoverflow.com/questions/19509348/sigpipe-osx-and-disconnected-sockets */
            int option_value = 1; /* Set NOSIGPIPE to ON */
            if (setsockopt (client, SOL_SOCKET, SO_NOSIGPIPE, &option_value, sizeof (option_value)) < 0)
                perror ("setsockopt(,,SO_NOSIGPIPE)");
        }
#endif

        std::thread client_thread(client_main, client);
        client_thread.detach();
    }
    close(server);

    return 0;
}

int create_socket(const std::string path)
{
#ifdef __APPLE__
    int server = socket(AF_UNIX, SOCK_STREAM, 0);
#else
    int server = socket(AF_UNIX, SOCK_SEQPACKET, 0);
#endif
    if (server < 0) {
        perror("Unable to create UNIX socket\n");
        abort();
    }

    struct sockaddr_un local;
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, path.c_str());
    unlink(local.sun_path);
    socklen_t len = sizeof(local);
    if (bind(server, (struct sockaddr*)(&local), len) < 0) {
        perror("Unable to bind to local daemon socket");
        abort();
    }

    if (listen(server, 5) < 0) {
        perror("Unable to listen");
        abort();
    }

    return server;
}

bool client_gone(int client)
{
    char c;
    ssize_t r = recv(client, &c, 1, MSG_PEEK | MSG_DONTWAIT);
    if (r == 0)
        return true;                    /* orderly shutdown by the peer */
    if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return false;                   /* connected, just nothing to read */
    if (r < 0)
        return true;                    /* real socket error -> treat as gone */
    return false;                       /* data pending -> still connected */
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
            /* Whatever it is that requested a sync is an event, so go wake up
             * all the waiters.  */
            std::unique_lock<std::mutex> lock(event_lock);
            auto ticket = ++event_ticket;
            std::cerr << "Notifying for event_ticket=" << std::to_string(ticket) << " due to SYNC.\n";
            event_signal.notify_all();
        } {
            /* Trigger the sync thread to go to the server and
             * actually do something. */
            std::unique_lock<std::mutex> lock(sync_lock);
            auto ticket = ++sync_req;
            sync_signal.notify_all();
            while (!(ticket <= sync_rep)) {
                if (client_gone(client)) { close(client); return; }
                sync_signal.wait_for(lock, client_wait_poll);
            }
        }
            break;

        case mhng::daemon::message_type::NET_UP:
        {
            fprintf(stderr, "Enabling Networking\n");

            std::unique_lock<std::mutex> lock(sync_lock);

            net_up = true;

            for (auto& idle_process: idle_processes)
                idle_process.kill();
            for (auto& sync_process: sync_processes)
                sync_process.kill();
            for (auto& oauth2_process: oauth2_processes)
                oauth2_process.kill();

            sync_signal.notify_all();

            break;
        }

        case mhng::daemon::message_type::NET_DOWN:
        {
            fprintf(stderr, "Disabling Networking\n");

            std::unique_lock<std::mutex> lock(sync_lock);

            net_up = false;

            for (auto& idle_process: idle_processes)
                idle_process.kill();
            for (auto& sync_process: sync_processes)
                sync_process.kill();
            for (auto& oauth2_process: oauth2_processes)
                oauth2_process.kill();

            sync_signal.notify_all();

            break;
        }

        case mhng::daemon::message_type::NEW_MESSAGE:
        {
            std::unique_lock<std::mutex> lock(sync_lock);
            auto uid = msg->new_message_uid();

            while (!(uid < sync_largest_uid)) {
                if (client_gone(client)) { close(client); return; }
                sync_signal.wait_for(lock, client_wait_poll);
            }
            break;
        }

        case mhng::daemon::message_type::FOLDER_EVENT:
        {
            std::unique_lock<std::mutex> lock(event_lock);
            auto ticket = msg->folder_event_ticket();

            /* event_ticket is guarded by event_lock and signalled on
             * event_signal, so wait on that cv rather than sync_signal. */
            while (!(ticket < event_ticket)) {
                if (client_gone(client)) { close(client); return; }
                event_signal.wait_for(lock, client_wait_poll);
            }
            std::cerr << "Finished FOLDER_EVENT(ticket=" << std::to_string(ticket) << ")\n";
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

        auto ticket_to_respond_with = [&]() -> uint32_t {
            std::unique_lock<std::mutex> lock(event_lock);
            return event_ticket;
        }();

        auto resp = msg->response(ticket_to_respond_with);
        auto len = mhng::daemon::message::serialize(resp, buf, BUFFER_SIZE);
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

void sync_main(mhng::daemon::process* sync_process)
{
    int retry_delay = retry_delay_min;

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
                sync_process->fork();

                /* Our ticket is just the ticket that the latest
                 * requester gave us.  This means that every request
                 * before we start will be satisfied! */
                return sync_req;
            };

        auto ticket = get_ticket_and_fork();
        int status = sync_process->join();

        /* If the synchronization didn't succeed then for now just
         * pretend it didn't happen at all and instead just go ahead
         * and try again.  Note that there's a simple rate limit in
         * here... */
        if (status != 0) {
            fprintf(stderr, "Synchronization failed, retrying in %d seconds\n",
                    retry_delay);
            wait_before_retry(retry_delay);
            continue;
        }

        retry_delay = retry_delay_min;

        /* After synchronization go and figure out what the largest
         * UID we've yet seen is. */
        auto last_uid = []() {
            std::unique_lock<std::mutex> lock(args_lock);
            return args->mbox()->largest_uid();
        }();

        /* Now that we've actually synchronized it's time to go ahead
         * and inform all the clients that we've done so. */
        {
            std::unique_lock<std::mutex> lock(sync_lock);
            if (sync_rep < ticket)
                sync_rep = ticket;
    
            if (last_uid > sync_largest_uid)
                sync_largest_uid = last_uid;
    
            sync_signal.notify_all();
        }

        /* After a sync we need to go ahead and fire off another event. */
        {
            std::unique_lock<std::mutex> lock(event_lock);
            event_ticket++;
            std::cerr << "Notifying for event_ticket=" << std::to_string(event_ticket) << " due to a sync finishing.\n";
            event_signal.notify_all();
        }
    }
}

void idle_main(mhng::daemon::process &idle_process, std::string account_name)
{
    int retry_delay = retry_delay_min;

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

            fprintf(stderr, "[%s] starting IDLE\n", account_name.c_str());
            idle_process.fork();
        }

        int status = idle_process.join();

        if (status != 0) {
            fprintf(stderr, "[%s] IDLE failed, retrying in %d seconds\n",
                    account_name.c_str(), retry_delay);
            wait_before_retry(retry_delay);
            continue;
        }

        retry_delay = retry_delay_min;
    }
}

void oauth2_main(mhng::daemon::process* oauth2_process)
{
    int retry_delay = retry_delay_min;

    while (true) {
        /* We only want to bother trying to oauth when we already know
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

            oauth2_process->fork();
        }

        int status = oauth2_process->join();
        if (status != 0) {
            fprintf(stderr, "OAUTH2 failed, retrying in %d seconds\n",
                    retry_delay);
            wait_before_retry(retry_delay);
            continue;
        }

        retry_delay = retry_delay_min;
    }
}
