/* Copyright (C) 2019 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__FAKE_MESSAGE_HXX
#define MHNG__FAKE_MESSAGE_HXX

#include <memory>

namespace mhng {
    class maybe_fake_message;
    typedef std::shared_ptr<maybe_fake_message> maybe_fake_message_ptr;

    class fake_message;
    typedef std::shared_ptr<fake_message> fake_message_ptr;
}

#include "mailbox.h++"
#include "message.h++"
#include "sequence_number.h++"

namespace mhng {
    /* Almost all code assumes we have a real message, but we sometimes need to
     * handle messages that don't actually exist.  While it's probably correct
     * to have these inherit from a common base, it's easier to just whack a
     * few dynamic switches into the places that actually care. */
    class maybe_fake_message {
    private:
        const message_ptr _real_message;
        const fake_message_ptr _fake_message;

    public:
        maybe_fake_message(const message_ptr& real_message)
        : _real_message(real_message),
          _fake_message(nullptr)
        {}

        maybe_fake_message(const fake_message_ptr& fake_message)
        : _real_message(nullptr),
          _fake_message(fake_message)
        {}

    public:
        const message_ptr real(void) const { return _real_message; }
        const fake_message_ptr fake(void) const { return _fake_message; }
    };

    /* Here's the actual fake message type, which just stores a sequence
     * number.  Fundamentally this exists because sometimes we want to refer
     * to a (folder, seq) pair that no longer exists. */
    class fake_message {
    private:
        const mailbox_ptr _mbox;
        const folder_ptr _folder;
        const sequence_number_ptr _seq;

    public:
        fake_message(const mailbox_ptr& mbox,
                     const folder_ptr& folder, 
                     const sequence_number_ptr& seq)
        : _mbox(mbox),
          _folder(folder),
          _seq(seq)
        {}

    public:
        /* Essentially the only thing you can do with a fake message is find a
         * real message, moving in some direction. */
        message_ptr next_message(int offset = 1);
    };
}

#endif
