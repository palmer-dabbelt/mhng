/* Copyright (C) 2016 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <string.h>
#include <termcap.h>
#include <algorithm>

static std::string remove_other_white(const std::string& in);
static uint32_t djb_hash(const std::string& in);

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_messages(argc, argv, {"+inbox"});

    auto env_date = [](){
        auto env = getenv("MHNG_HFIPIP_DATE");
        if (env != nullptr)
            return std::string(env);
        else
            return mhng::date::now()->local();
    }();

    /* At this point that argument list contains the entire set of
     * messages that should be examined as part of the scan. */
    printf("<HTML> <HEAD> <TITLE> HFIPI? </TITLE> </HEAD> <BODY>\n");

    printf("This page computes the DJB hash of the subjects of all the messages in my inbox every minute (when my computer is on), so you can see if I've lost your message or not.  The message I most recently viewed is marked with a \"*\", and unread messages are marked with a \"U\".<br/>\n");
    printf("<p/>\n");

    for (const auto& msg: args->messages()) {
        auto subj = djb_hash(remove_other_white(msg->first_subject()));
        auto from = djb_hash(remove_other_white(msg->first_from()->email()));

        printf("<tt>%s&nbsp%s&nbsp%s&nbsp0x%08x&nbsp0x%08x</tt><br/>\n",
               msg->cur() ? "*" : "&nbsp",
               msg->unread() ? "U" : "&nbsp",
               msg->first_date()->ddmm().c_str(),
               from,
               subj
            );
    }
    printf("<p/>\n");
    printf("This message was computed on %s\n", env_date.c_str());
    printf("</BODY> </HTML>\n");

    return 0;
}

std::string remove_other_white(const std::string& in)
{
    std::string out(in.length(), ' ');

    std::transform(in.begin(), in.end(), out.begin(),
                   [](char inc) -> char
                   {
                       if (isspace(inc))
                           return ' ';
                       return inc;
                   });

    return out;
}

uint32_t djb_hash(const std::string& in)
{
    uint32_t hash = 5381;

    for (const char c: in)
        hash = ((hash << 5) + hash) + c;

    return hash;
}
