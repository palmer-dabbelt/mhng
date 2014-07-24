
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

#include <libmhng/args.h++>
#include <libmhng/mime/part.h++>
#include <functional>
#include <string.h>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_numbers(argc, argv);
    args->mbox()->set_current_folder(args->folders()[0]);
    args->folders()[0]->set_current_message(args->messages()[0]);

    /* The first thing we want to do is build up a vector of the
     * MIME parts that are availiable to this message. */
    std::vector<mhng::mime::part_ptr> parts;
    std::map<mhng::mime::part_ptr, size_t> part2depth;
    for (const auto& msg: args->messages()) {
        int depth = 0;

        auto root = msg->mime()->root();

        std::function<void(const mhng::mime::part_ptr&)> fill =
                      [&](const mhng::mime::part_ptr& part) -> void
            {
                parts.push_back(part);
                part2depth[part] = depth;

                depth++;
                for (const auto& child: part->children())
                    fill(child);
                depth--;
            };

        fill(root);
    }

    /* If we were given any arbitrary numbers on the commandline then
     * go ahead and print out the cooresponding MIME parts to
     * stdout. */
    for (auto num: args->numbers()) {
        if (num > (int)(parts.size())) {
            fprintf(stderr, "Requested MIME %d, but only %d parts\n",
                    num, (int)(parts.size())
                );
            abort();
        }

        /* FIXME: Output the decoded data here, as the raw stuff is
         * actually pretty worthless as it explicitly doesn't decode
         * anything (making the output attachments not useful at
         * all). */
        auto part = parts[num - 1];
        for (const auto& raw: part->body_raw())
            printf("%s", raw.c_str());
    }
    if (args->numbers().size() != 0)
        return 0;

    /* If we weren't given any numbers on the commandline then print
     * out every MIME part that the message contains in a nice little
     * tree format. */
    for (size_t i = 0; i < parts.size(); ++i) {
        auto part = parts[i];
        printf("%lu ", i+1);

        for (size_t depth = 0; depth < part2depth[part]; ++depth)
            printf("  ");

        printf("%s (%s)\n",
               part->name().c_str(),
               part->content_type().c_str()
            );
    }

    return 0;
}
