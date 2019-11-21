/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "folder.h++"
#include "fake_message.h++"
#include "db/mh_current.h++"
#include "db/mh_messages.h++"
#include "db/imap_messages.h++"
#include "db/imap_uidvalidity.h++"
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

folder::folder(const mailbox_ptr& mbox,
               std::string name)
    : _mbox(mbox),
      _name(name),
      _current_message(this, _current_message_func),
      _messages(this, _messages_func)
{
}

size_t folder::message_count(void)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    return messages->count(this->name());
}

message_ptr folder::open(uint64_t uid)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    return messages->select(uid);
}

message_ptr folder::open(const sequence_number_ptr& seq)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    return messages->select(this->name(), seq);
}

std::string folder::full_path(void) const
{
    char path[BUFFER_SIZE];

    if (getenv("MHNG_MAILDIR") != NULL) {
        snprintf(path, BUFFER_SIZE, "%s/mail/%s",
                 getenv("MHNG_MAILDIR"),
                 _name.c_str()
            );
        return path;
    }

    snprintf(path, BUFFER_SIZE, "%s/.mhng/mail/%s",
             getenv("HOME"),
             _name.c_str()
        );
    return path;
}

void folder::set_current_message(const message_ptr& message)
{
    auto cur = std::make_shared<db::mh_current>(_mbox);
    cur->update(message->folder()->name(), message->seq()->to_uint());
    _current_message = message;
}

message_ptr folder::open_imap(uint32_t imapid, const account_ptr& account)
{
    auto imap = std::make_shared<db::imap_messages>(_mbox);
    auto uid = imap->select(this->name(), imapid, account->name());

    auto mh = std::make_shared<db::mh_messages>(_mbox);
    auto message = mh->select(uid);

    if (message != NULL)
        message->set_imapid(imapid);

    return message;
}

bool folder::has_uid_validity(const account_ptr& account)
{
    auto table = std::make_shared<db::imap_uidvalidity>(_mbox);
    auto out = table->select(_name, account->name());
    return out.known();
}

uint32_t folder::uid_validity(const account_ptr& account)
{
    auto table = std::make_shared<db::imap_uidvalidity>(_mbox);
    auto out = table->select(_name, account->name());
    return out.data();
}

void folder::set_uid_validity(uint32_t uidv, const account_ptr& account)
{
	auto table = std::make_shared<db::imap_uidvalidity>(_mbox);
    table->insert(_name, uidv, account->name());
}

message_ptr folder::_current_message_impl(void)
{
    auto cur = std::make_shared<db::mh_current>(_mbox);
    auto seq_uint = cur->select(_name);
    auto seq = std::make_shared<sequence_number>(seq_uint);

    auto msgs = std::make_shared<db::mh_messages>(_mbox);
    auto msg = msgs->select(_name, seq);

    return msg;
}

std::shared_ptr<std::vector<message_ptr>> folder::_messages_impl(void)
{
    auto tr = _mbox->db()->deferred_transaction();

    auto out = std::make_shared<std::vector<message_ptr>>();
    auto table = std::make_shared<db::mh_messages>(_mbox);

    for (const auto& message: table->select(this->name()))
        out->push_back(message);

    return out;
}

fake_message_ptr folder::fake_current_message(void)
{
    auto cur = std::make_shared<db::mh_current>(_mbox);
    auto seq_uint = cur->select(_name);
    auto seq = std::make_shared<sequence_number>(seq_uint);

    return std::make_shared<fake_message>(_mbox, shared_from_this(), seq);
}

std::vector<uint32_t> folder::purge(const account_ptr& account)
{
    auto table = std::make_shared<db::imap_messages>(_mbox);
    auto out = std::vector<uint32_t>();
    for (const auto& uid: table->select_purge(_name, account->name()))
        out.push_back(uid);
    return out;
}
