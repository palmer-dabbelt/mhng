
/*
 * Copyright (C) 2013 Palmer Dabbelt
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

#include "client.h++"
#include "done_command.h++"
#include "logger.h++"
#include <string.h>

using namespace mhimap;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

client::client(void)
    : sequence(0),
      logged_out(false)
{
}

typename mhimap::folder_iter client::folder_iter(void)
{
    logger l("client::folder_iter()");

    std::vector<folder> folders;

    l.printf("list(...)");
    command list(this, "LIST / *");

    char buffer[BUFFER_SIZE];
    while (gets(buffer, BUFFER_SIZE) > 0) {
        if (list.is_end(buffer))
            break;

        if (!list.is_list(buffer)) {
            fprintf(stderr, "Spurious non-LIST: '%s'\n", buffer);
            continue;
        }

        std::string folder_name(list.list_get_folder(buffer));
        folder f(folder_name);
        folders.push_back(f);
    }

    return mhimap::folder_iter(folders);
}

string_iter client::message_iter(const folder f)
{
    const std::string folder_name(f.name());

    logger l("client::message_iter('%s')", folder_name.c_str());
    char buffer[BUFFER_SIZE];

    /* FIXME: I at least need to look at UIDVALIDITY here...  I think
     * I should probably not be returning folders and messages as
     * strings but should instead have wrapper classes. */
    command select(this, "SELECT %s", folder_name.c_str());
    while (gets(buffer, BUFFER_SIZE) > 0) {
        if (select.is_end(buffer))
            break;
    }

    /* This just selects every message in the inbox and passes that
     * exact string along to the iterator we're passing out. */
    std::vector<std::string> messages;

    command fetch(this, "UID FETCH 1:* (UID)");
    while (gets(buffer, BUFFER_SIZE) > 0) {
        if (fetch.is_end(buffer))
            break;

        messages.push_back(buffer);
    }

    return string_iter(messages);
}

void client::send_idle(const std::string folder_name)
{
    logger l("client::send_idle('%s')", folder_name.c_str());
    char buffer[BUFFER_SIZE];

    l.printf("select(...)");
    command select(this, "SELECT %s", folder_name.c_str());
    do {
        l.printf("gets(...)");
        gets(buffer, BUFFER_SIZE);

        l.printf("is_end(...)");
    } while (!select.is_end(buffer));

    l.printf("idle(...)");
    command idle(this, "IDLE");
    gets(buffer, BUFFER_SIZE);
    /* FIXME: I should probably check if the IDLE is OK or not... */
}

idle_response client::wait_idle(void)
{
    logger l("client::wait_idle()");
    char buffer[BUFFER_SIZE];

    /* Here we just wait for _anything_ to come back, at which point
     * we're safe to return. */
    gets(buffer, BUFFER_SIZE);

    /* Now we issue a DONE response to the server, which indicates
     * that we don't want to IDLE any more -- note that this isn't
     * _quite_ a regular command yet because it doesn't get tagged
     * with a number.  */
    done_command done(this);

    /* At this point we need to drain the server's event queue and
     * check that everything was OK with our return. */
    while (!done.is_end(buffer))
        gets(buffer, BUFFER_SIZE);

    return idle_response::DATA;
}

int client::eat_hello(void)
{
    logger("client::eat_hello()");

    char buffer[BUFFER_SIZE];
    if (gets(buffer, BUFFER_SIZE) < 0) {
        fprintf(stderr, "Unable to eat a hello line from server\n");
        abort();
    }

    if (was_error(buffer)) {
        return -1;
    }

    return 0;
}

int client::authenticate(const std::string user, const std::string pass)
{
    char buffer[BUFFER_SIZE];
    logger l("client::authenticate('%s', ...)", user.c_str());

    l.printf("login(...)");
    command login(this, "LOGIN %s %s", user.c_str(), pass.c_str());
    do {
        l.printf("gets(...)");
        gets(buffer, BUFFER_SIZE);

        l.printf("is_end(...)");
    } while (!login.is_end(buffer));

    if (login.is_error_end(buffer)) {
        fprintf(stderr, "Error while logging in: '%s'\n", buffer);
        abort();
    }

    return 0;
}

void client::logout(void)
{
    char buffer[BUFFER_SIZE];
    logger l("client::~client()");

    command logout(this, "LOGOUT");

    do {
        l.printf("gets(...)");
        gets(buffer, BUFFER_SIZE);

        l.printf("is_end(...)");
    } while (!logout.is_end(buffer));

    if (logout.is_error_end(buffer)) {
        fprintf(stderr, "Error while logging out: '%s'\n", buffer);
    }
}

ssize_t client::puts(const char *buffer)
{
    logger l("client::puts('%s')", buffer);

    char nbuffer[BUFFER_SIZE + 3];

    l.printf("snprintf(...)");
    snprintf(nbuffer, BUFFER_SIZE + 3, "%s\r\n", buffer);

    l.printf("write(...)");
    write(nbuffer, strlen(nbuffer));

#ifdef IMAP_NETLOG
    fprintf(stderr, "%p >> %s\n", (void*)this, buffer);
#endif

    return strlen(nbuffer);
}

ssize_t client::gets(char *buffer, ssize_t buffer_size)
{
    logger l("client::gets(...)");

    while (!linebuf.has_line()) {
        l.printf("read(...)");
        ssize_t n_read = read(linebuf.recharge_buffer(),
                              linebuf.recharge_size());

        l.printf("read   ==> '%*s'", (int)n_read, linebuf.recharge_buffer());
        l.printf("n_read ==> %ld", n_read);

        /* Just die on any errors. */
        if (n_read < 0) {
            fprintf(stderr, "Read failure\n");
            abort();
        }

        linebuf.recharge_commit(n_read);
    }

    ssize_t out = linebuf.get(buffer, buffer_size);
    l.printf("==> '%s'", buffer);

#ifdef IMAP_NETLOG
    fprintf(stderr, "%p << %s\n", (void*)this, buffer);
#endif

    return out;
}

bool client::was_error(char *buffer)
{
    if (strncmp(buffer, "* OK ", strlen("* OK ")) == 0)
        return false;

    return true;
}
