/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "mailbox.h++"
#include "mailrc.h++"
#include "daemon/dummy_connection.h++"
#include "daemon/real_connection.h++"
#include "db/mh_messages.h++"
#include "db/mh_current.h++"
#include "db/mh_nextid.h++"
#include "db/mh_oauth2cred.h++"
#include "db/mh_accounts.h++"
#include "db/mh_userpass.h++"
#include "db/imap_messages.h++"
#include <libmhoauth/pkce.h++>
#include <chrono>
#include <sstream>
#include <string.h>
#include <time.h>
#include <unistd.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

mailbox::mailbox(const std::string& path, bool nomailrc)
    : _path(path),
      _nomailrc(nomailrc),
      _db(std::make_shared<psqlite::connection>(path + "/metadata.sqlite3")),
      _current_folder(this, _current_folder_func),
      _mailrc(this, _mailrc_func),
      _daemon(this, _daemon_func),
      _accounts(this, _accounts_func)
{
}

folder_ptr mailbox::open_folder(std::string folder_name) const
{
    /* Check to see that this folder exists before trying to return
     * it. */
    auto folder_path = _path + "/mail/" + folder_name;
    if (access(folder_path.c_str(), X_OK) != 0)
        return NULL;

    /* Now that we know we have access to the folder it's time to
     * simply return a handle for that folder. */
    return std::make_shared<folder>(_self_ptr.lock(), folder_name);
}

account_ptr mailbox::account(const std::string& name)
{
    auto table = std::make_shared<db::mh_accounts>(_self_ptr.lock());
    return table->select(name);
}

/* Returns every account that's currently supported. */
std::vector<account_ptr> mailbox::accounts(void)
{
    std::vector<account_ptr> out;
    auto acc = _accounts.get();
    for (auto account = acc->begin(); account != acc->end(); ++account) {
        auto ac = *account;
        out.push_back(ac);
    }
    return out;
}

void mailbox::set_current_folder(const folder_ptr& folder)
{
    /* Check to see if the current folder matches the folder we've
     * requented to move to and if so just don't worry about issuing
     * the update.  Note that this is racy, but who cares: if you want
     * this to actually work then you'll have to wrap everything in a
     * transaction anyway, so it's not like it matters.  This has the
     * huge advantage of not doing writes all the time, so I think
     * it's worth it! */
    if (current_folder()->name() == folder->name())
        return;

    auto tr = _db->deferred_transaction();

    auto table = std::make_shared<db::mh_current>(_self_ptr.lock());
    table->clear_current(current_folder()->name());
    table->set_current(folder->name());
    _current_folder = folder;
}

message_ptr mailbox::insert(const std::string &folder_name,
                            const mime::message_ptr& mime)
{
    /* This whole thing needs to be locked... */
    auto trans = _db->deferred_transaction();

    /* Find a new unique MH id for this message. */
    auto idtbl = std::make_shared<db::mh_nextid>(_self_ptr.lock());
    auto uid = idtbl->select();
    idtbl->update(uid + 1);

    /* Fin a new sequence number for this message. */
    auto folder = open_folder(folder_name);
    unsigned seq = 1;
    for (const auto& message: folder->messages())
        if (message->seq()->to_uint() >= seq)
            seq = message->seq()->to_uint() + 1;

    /* Determine some other relevant information for this message. */
    std::shared_ptr<date> date = NULL;
    std::shared_ptr<address> from = NULL;
    std::shared_ptr<address> to = NULL;
    std::string subject = "";

    /* Walk through the headers to try and fill out some relevant
     * fields. */
    for (const auto& header: mime->header("Date")) {
        auto ho = header->utf8_maybe();
        if (ho.has_value())
            date = std::make_shared<mhng::date>(ho.value());
        else
            fprintf(stderr, "Unable to parse Date: %s\n", mime->debug().c_str());
    }
    for (const auto& header: mime->header("From")) {
        auto ho = header->utf8_maybe();
        if (ho.has_value())
            from = address::parse_rfc(ho.value(), false);
        else
            fprintf(stderr, "Unable to parse From: %s\n", mime->debug().c_str());
    }
    for (const auto& header: mime->header("To")) {
        auto ho = header->utf8_maybe();
        if (ho.has_value())
            to = address::parse_rfc(ho.value(), false);
        else
            fprintf(stderr, "Unable to parse To: %s\n", mime->debug().c_str());
    }
    for (const auto& header: mime->header("Subject")) {
        auto ho = header->utf8_maybe();
        if (ho.has_value())
            subject = ho.value();
        else
            fprintf(stderr, "Unable to parse Subject: %s\n", mime->debug().c_str());
    }

    if (from == NULL)
        from = address::parse_rfc("", false);
    if (to == NULL)
        to = address::parse_rfc("", false);

    from = mrc()->email(from->email());
    to = mrc()->email(to->email());

    auto msg = std::make_shared<message>(_self_ptr.lock(),
                                         std::make_shared<sequence_number>(seq),
                                         folder,
                                         date,
                                         from,
                                         to,
                                         subject,
                                         std::to_string(uid),
                                         0
        );

    auto messages = std::make_shared<db::mh_messages>(_self_ptr.lock());
    messages->insert(seq,
                     folder->name(),
                     std::to_string(date->unix()),
                     from->email_known() ? from->email() : "?",
                     to->email_known()   ? to->email()   : "?",
                     subject,
                     uid
        );

    char filename[BUFFER_SIZE];
    snprintf(filename, BUFFER_SIZE, "%s/%s",
             folder->full_path().c_str(),
             std::to_string(uid).c_str());
             
    FILE *file = fopen(filename, "w");
    for (const auto& raw: mime->root()->raw())
        fprintf(file, "%s", raw.c_str());
    fflush(file);
    fsync(fileno(file));
    fclose(file);

    if (folder->current_message() == NULL)
        folder->set_current_message(msg);

    return msg;
}

message_ptr mailbox::insert(const folder_ptr& folder,
                            const mime::message_ptr& mime,
                            uint32_t imapid,
                            const std::string account)
{
    auto trans = _db->deferred_transaction();

    auto message = insert(folder->name(), mime);

    uint64_t uid = atoi(message->uid().c_str());

    auto table = std::make_shared<db::imap_messages>(_self_ptr.lock());
    table->insert(message->folder()->name(), imapid, uid, account);

    return folder->open(uid);
}

void mailbox::did_purge(const folder_ptr& folder, uint32_t imapid, const account_ptr& account)
{
    auto table = std::make_shared<db::imap_messages>(_self_ptr.lock());
    table->remove(folder->name(), imapid, account->name());
}

uint64_t mailbox::largest_uid(void) const
{
    auto current = std::make_shared<db::mh_current>(_self_ptr.lock());

    uint64_t largest = 0;
    for (const auto& folder_name: current->select()) {
        auto folder_largest = largest_uid(folder_name);
        if (folder_largest > largest)
            largest = folder_largest;
    }

    return largest;
}

uint64_t mailbox::largest_uid(const std::string folder) const
{
    auto table = std::make_shared<db::mh_messages>(_self_ptr.lock());

    uint64_t largest = 0;
    for (const auto& message: table->select(folder)) {
        uint64_t uid;
        std::istringstream(message->uid()) >> uid;
        if (uid > largest)
            largest = uid;
    }

    return largest;
}

message_ptr mailbox::open(uint64_t uid) const
{
    auto table = std::make_shared<db::mh_messages>(_self_ptr.lock());
    return table->select(uid);
}

void mailbox::add_oauth2_account(const std::string& name) const
{
    if (getenv("MHNG_OAUTH2_CLIENT_ID") == nullptr) {
        std::cerr << "Set $MHNG_OAUTH2_CLIENT_ID\n";
        abort();
    }

    auto access_token = libmhoauth::pkce(
        getenv("MHNG_OAUTH2_CLIENT_ID"),
        "https://accounts.google.com/o/oauth2/v2/auth",
        "https://oauth2.googleapis.com/token",
        getenv("BROWSER"),
        "http://mail.google.com",
        name
    );

    {
        auto table = std::make_shared<db::mh_oauth2cred>(_self_ptr.lock());
        auto ts = [&](){
            auto tp = access_token.expires_at();
            auto ss = std::chrono::time_point_cast<std::chrono::seconds>(tp);
            auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp)
                      - std::chrono::time_point_cast<std::chrono::nanoseconds>(ss);
            timespec out;
            out.tv_sec = ss.time_since_epoch().count();
            out.tv_nsec = ns.count();
            return out;
        }();
        table->insert(name,
                      getenv("MHNG_OAUTH2_CLIENT_ID"),
                      access_token.value(),
                      access_token.refresh_token(),
                      putil::chrono::datetime(ts));
    }

    {
        auto table = std::make_shared<db::mh_accounts>(_self_ptr.lock());
        table->insert(name, "oauth2");
    }
}

void mailbox::add_userpass_account(const std::string& name) const
{
    {
        std::cout << "password:\n";
        std::string pass = [](){
            std::string out;
            std::getline(std::cin, out);
            return out;
        }();
        auto table = std::make_shared<db::mh_userpass>(_self_ptr.lock());
        table->insert(name, pass);
    }

    {
        auto table = std::make_shared<db::mh_accounts>(_self_ptr.lock());
        table->insert(name, "userpass");
    }
}

void mailbox::redo_account_oauth(const std::string& name) const
{
    auto access_token = libmhoauth::pkce(
        getenv("MHNG_OAUTH2_CLIENT_ID"),
        "https://accounts.google.com/o/oauth2/v2/auth",
        "https://oauth2.googleapis.com/token",
        getenv("BROWSER"),
        "http://mail.google.com",
        name
    );

    auto table = std::make_shared<db::mh_oauth2cred>(_self_ptr.lock());
    auto ts = [&](){
        auto tp = access_token.expires_at();
        auto ss = std::chrono::time_point_cast<std::chrono::seconds>(tp);
        auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp)
                  - std::chrono::time_point_cast<std::chrono::nanoseconds>(ss);
        timespec out;
        out.tv_sec = ss.time_since_epoch().count();
        out.tv_nsec = ns.count();
        return out;
    }();
    table->get_new_grant(name,
                         getenv("MHNG_OAUTH2_CLIENT_ID"),
                         access_token.value(),
                         access_token.refresh_token(),
                         putil::chrono::datetime(ts));
}

folder_ptr mailbox::_current_folder_impl(void)
{
    auto table = std::make_shared<db::mh_current>(_self_ptr.lock());
    return open_folder(table->select_current());
}

mailrc_ptr mailbox::_mailrc_impl(void)
{
    if (_nomailrc)
        return std::make_shared<typename mhng::mailrc>("/dev/null");

    if (getenv("MHNG_MAILRC") != NULL)
        return std::make_shared<typename mhng::mailrc>(getenv("MHNG_MAILRC"));

    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/.mailrc",
             getenv("HOME"));
    return std::make_shared<typename mhng::mailrc>(path);
}

daemon::connection_ptr mailbox::_daemon_impl(void)
{
    if (getenv("MHNG_NO_DAEMON") != NULL) {
        fprintf(stderr, "MHNG_NO_DAEMON set, using no sync daemon\n");
        return std::make_shared<daemon::dummy_connection>();
    }

    return std::make_shared<daemon::real_connection>(_path + "/daemon.socket");
}

std::shared_ptr<std::vector<account_ptr>> mailbox::_accounts_impl(void)
{
    auto table = std::make_shared<db::mh_accounts>(_self_ptr.lock());
    auto out = std::make_shared<std::vector<account_ptr>>();
    for (const auto& account: table->select())
        out->push_back(account);
    return out;
}


