
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

#include "address.h++"
#include <string.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

address::address(const unknown<std::string>& email,
                 const unknown<std::string>& name,
                 const unknown<std::string>& alias,
                 bool local)
    : _email(email),
      _name(name),
      _alias(alias),
      _local(local)
{
}

std::string address::nom(void) const
{
    if (_name.known() == true)
        return _name.data();
    return _email.data();
}

std::string address::rfc(void) const
{
    if (_name.known() && _email.known()) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%s <%s>",
                 _name.data().c_str(),
                 _email.data().c_str()
            );
        return buffer;
    }

    if (_email.known())
        return _email.data();

    return "?";
}

address_ptr address::from_email(const std::string email,
                                bool local)
{
    /* Email addresses must have an @, otherwise they're not valid. */
    if (strstr(email.c_str(), "@") == NULL) {
        return std::make_shared<address>(
            unknown<std::string>(),
            unknown<std::string>(),
            unknown<std::string>(),
            local
            );
    }

    return std::make_shared<address>(
        unknown<std::string>(email),
        unknown<std::string>(),
        unknown<std::string>(),
        local
        );
}

address_ptr address::parse_alias(const std::string rfc,
                                 const unknown<std::string>& alias,
                                 bool local)
{
    if (strstr(rfc.c_str(), "<") == NULL) {
        return std::make_shared<address>(
            unknown<std::string>(rfc),
            unknown<std::string>(),
            alias,
            local
            );
    }

    char buf[BUFFER_SIZE];
    snprintf(buf, BUFFER_SIZE, "%s", rfc.c_str());

    /* Here we seperate out the mail and strip the < > from it. */
    auto mail_start = strstr(buf, "<");
    if (strstr(mail_start, ">") == NULL) {
        fprintf(stderr, "Found mail with <, but not >, full string is '%s'\n", rfc.c_str());
        abort();
    }
   
    strstr(mail_start, ">")[0] = '\0';
    mail_start++;
    mail_start[-1] = '\0';

    /* Now we strip out the name to figure out what to call that. */
    auto name_start = buf;
    while (isspace(name_start[0]))
        name_start++;
    while (strlen(name_start) > 0 && isspace(name_start[strlen(name_start)-1]))
        name_start[strlen(name_start)-1] = '\0';

    /* It's possible that names start with some sort of quotation, if
     * so skip that. */
    if (name_start[0] == '"')
        name_start++;
    if (strlen(name_start) > 0 && name_start[strlen(name_start)-1] == '"')
        name_start[strlen(name_start)-1] = '\0';

    return std::make_shared<address>(
        unknown<std::string>(mail_start),
        unknown<std::string>(name_start),
        alias,
        local
        );
}
