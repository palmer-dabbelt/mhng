/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__ADDRESS_HXX
#define MHNG__ADDRESS_HXX

#include <memory>

namespace mhng {
    class address;
    typedef std::shared_ptr<address> address_ptr;
}

#include "unknown.h++"
#include <string>

namespace mhng {
    /* This holds an address that's been properly parsed.  In other
     * words, it's not just a plain string but instead is actually a
     * wrapper that contains a whole bunch of strings! */
    class address {
    private:
        unknown<std::string> _email;
        unknown<std::string> _name;
        unknown<std::string> _alias;
        bool _local;

    public:
        /* The only way to create an address is to have the entire
         * address parsed.  The only reason this is public because I
         * need to make shared pointers from this... */
        address(const unknown<std::string>& email,
                const unknown<std::string>& name,
                const unknown<std::string>& alias,
                bool local);

    public:
        /* Allows access to the internal fields (pretty much)
         * directly. */
        bool email_known(void) const { return _email.known(); }
        std::string email(void) const { return _email.data(); }
        bool name_known(void) const { return _name.known(); }
        std::string name(void) const { return _name.data(); }
        bool alias_known(void) const { return _alias.known(); }
        std::string alias(void) const { return _alias.data(); }
        bool local(void) const { return _local; }

        /* Returns either the name or email address for this address.
         * Note that this always succeeds */
        std::string nom(void) const;

        /* Returns the full RFC representation of this address. */
        std::string rfc(void) const;

    public:
        /* Parses a raw email address.  This doesn't do any checks at
         * all, so you'll probably want to be very sure */
        static address_ptr from_email(const std::string email, bool local);

        /* Parses the sort of line you might see in a mail header. */
        static address_ptr parse_rfc(const std::string rfc, bool local)
            { return parse_alias(rfc, unknown<std::string>(), local); }

        /* Exactly like parse_rfc(), but might contain an alias. */
        static address_ptr parse_alias(const std::string rfc,
                                       const unknown<std::string>& alias,
                                       bool local);
        static address_ptr parse_alias(const std::string rfc,
                                       const std::string alias,
                                       bool local)
            { return parse_alias(rfc, unknown<std::string>(alias), local); }
    };
}

#endif
