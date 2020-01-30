/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "message.h++"
#include "db/imap_messages.h++"
#include "db/mh_current.h++"
#include "db/mh_messages.h++"
#include "db/imap_messages.h++"
#include <set>
#include <string.h>
#include <unistd.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Returns TRUE if the given message is the current message and FALSE
 * otherwise. */
static bool check_for_current(const mailbox_ptr& mbox,
                              const folder_ptr& folder,
                              const unsigned& seq);
static unknown<uint32_t> check_for_imapid(const mailbox_ptr& mbox,
                                          const std::string& uid);

message::message(const mailbox_ptr& mbox,
                 const sequence_number_ptr& seq,
                 const folder_ptr& folder,
                 const date_ptr& date,
                 const address_ptr& from,
                 const address_ptr& to,
                 const std::string& subject,
                 const std::string& uid,
                 int unread)
    : _mbox(mbox),
      _cur(check_for_current(mbox, folder, seq->to_uint())),
      _seq(seq),
      _folder(folder),
      _date(date),
      _from(from),
      _to(to),
      _subject(subject),
      _uid(uid),
      _unread(unread),
      _imapid(check_for_imapid(mbox, uid)),
      _raw(this, _raw_func),
      _mime(this, _mime_func)
{
}

std::string message::imap_account_name(void)
{
    auto table = std::make_shared<db::imap_messages>(_mbox);
    return table->select_account(atoi(_uid.c_str()));
}

void message::remove(void)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    messages->remove(atoi(_uid.c_str()));

    auto imap = std::make_shared<db::imap_messages>(_mbox);
    imap->update_purge(atoi(_uid.c_str()), true);

    int err = unlink(full_path().c_str());
    if (err < 0)
        perror("Unable to remove message file, your mailbox may be corrupted");
}

std::vector<address_ptr> message::header_addr(const std::string name) const
{
    std::vector<address_ptr> out;

    for (const auto& hdr: header(name)) {
        for (const auto& adstr: hdr->split_commas()) {
            auto addr = address::parse_rfc(adstr, false);
            auto lookup = _mbox->mrc()->email(addr->email());
            out.push_back(lookup);
        }
    }

    return out;
}

std::vector<std::string> message::header_string(const std::string name) const
{
    std::vector<std::string> out;

    for (const auto& hdr: header(name)) {
        auto str = hdr->utf8();
        out.push_back(str);
    }

    return out;
}

std::vector<date_ptr> message::header_date(const std::string name) const
{
    std::vector<date_ptr> out;

    for (const auto& hdr: header(name)) {
        auto dstr = hdr->utf8();
        auto d = std::make_shared<mhng::date>(dstr);
        out.push_back(d);
    }

    return out;
}

std::vector<mime::header_ptr> message::header(const std::string name) const
{
    return mime()->header(name);
}

std::vector<std::string> message::header_ids(const std::string name) const
{
    std::vector<std::string> out;

    for (const auto& hdr: header(name))
        for (const auto& str: hdr->split_id())
            out.push_back(str);

    return out;
}

message_ptr message::next_message(int offset)
{
    auto table = std::make_shared<db::mh_messages>(_mbox);
    return table->select(_folder->name(), _seq, offset);
}

std::string message::full_path(void) const
{
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/%s",
             _folder->full_path().c_str(),
             _uid.c_str()
        );
    return path;
}

void message::set_sequence_number(const sequence_number_ptr& seq)
{
    auto tr = _mbox->db()->deferred_transaction();

    if (_seq->to_uint() == seq->to_uint())
        return;

    auto messages = std::make_shared<db::mh_messages>(_mbox);
    messages->update(atoi(_uid.c_str()), seq);

    auto current = std::make_shared<db::mh_current>(_mbox);
    current->update(_folder->name(), seq->to_uint());
}

void message::mark_read_and_unsynced(void)
{
    _unread = 1;

    auto messages = std::make_shared<db::mh_messages>(_mbox);
    messages->update_unread(atoi(_uid.c_str()), _unread);
}

void message::mark_read_and_synced(void)
{
    _unread = 2;

    auto messages = std::make_shared<db::mh_messages>(_mbox);
    messages->update_unread(atoi(_uid.c_str()), _unread);
}

std::vector<message_ptr> message::get_messages_in_thread(void) const
{
    auto message_id = [](auto msg) {
            auto headers = msg->header_string("Message-ID");
            if (headers.size() != 1)
                abort();
            return headers[0];
        };

    /* Message IDs that are part of the thread. */
    auto message_ids = [&]()
        {
            auto out = std::set<std::string>();
            auto headers = header_string("Message-ID");
            if (headers.size() != 1)
                abort();
            out.insert(headers[0]);

            for (const auto& ref: header_ids("References"))
                out.insert(ref);
            for (const auto& ref: header_ids("In-Reply-To"))
                out.insert(ref);

            return out;
        }();
    std::map<std::string, message_ptr> mid2msg;
    auto modified = true;
    auto messages = _folder->messages();

    auto in_thread = [&](auto msg) {
        auto id = message_id(msg);
        if (mid2msg.find(id) != mid2msg.end())
            return;

        modified = true;
        mid2msg[id] = msg;
        for (const auto& ref: msg->header_ids("References"))
            message_ids.insert(ref);
        for (const auto& ref: msg->header_ids("In-Reply-To"))
            message_ids.insert(ref);
    };

    while (modified) {
        modified = false;

        for (const auto& msg: messages) {
            for (const auto& ref: msg->header_ids("References"))
                if (message_ids.find(ref) != message_ids.end())
                    in_thread(msg);
            for (const auto& ref: msg->header_ids("In-Reply-To"))
                if (message_ids.find(ref) != message_ids.end())
                    in_thread(msg);
        }
    }

    for (const auto& msg: messages)
        msg->compact();

    return [&](){
        std::vector<message_ptr> out;
        for (const auto& pair: mid2msg)
            out.push_back(pair.second);
        return out;
    }();
}

std::shared_ptr<std::vector<std::string>> message::_raw_impl(void)
{
    FILE *file = fopen(full_path().c_str(), "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open message: '%s'\n",
                full_path().c_str());
        abort();
    }

    auto out = std::make_shared<std::vector<std::string>>();
    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file) != NULL)
        out->push_back(line);

    fclose(file);
    return out;
}

std::shared_ptr<mime::message> message::_mime_impl(void)
{
    return std::make_shared<mime::message>(this->raw());
}

bool check_for_current(const mailbox_ptr& mbox,
                       const folder_ptr& folder,
                       const unsigned& seq)
{
    auto table = std::make_shared<db::mh_current>(mbox);
    return table->select(folder->name()) == seq;
}

unknown<uint32_t> check_for_imapid(const mailbox_ptr& mbox,
                                   const std::string& uid)
{
    auto table = std::make_shared<db::imap_messages>(mbox);
    auto imapid = table->select(atoi(uid.c_str()));
    if (imapid >= 0)
        return unknown<uint32_t>(imapid);
    else
        return unknown<uint32_t>();
}
