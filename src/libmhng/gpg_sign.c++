
/*
 * Copyright (C) 2013,2014 Palmer Dabbelt
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

#ifdef HAVE_GPGME
#include "gpg_sign.h++"
#include <gpgme.h>
#include <string.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

std::vector<std::string> mhng::gpg_sign(const std::vector<std::string>& in,
                                        std::string email)
{
    gpgme_check_version(NULL);

    gpgme_ctx_t ctx;
    if (gpgme_new(&ctx) != 0) {
        perror("Unable to init GPG\n");
        abort();
    }

    /* These take the input as text, and produce the output as
     * ASCII-armored text. */
    gpgme_set_textmode(ctx, 1);
    gpgme_set_armor(ctx, 1);

    /* Set the signer of this message based on the given email
     * address, trying our best to find a matching key. */
    gpgme_signers_clear(ctx);
    {
        bool found_key = false;
        gpgme_key_t key;
        gpgme_op_keylist_start(ctx, email.c_str(), 1);
        while (gpgme_op_keylist_next(ctx, &key) == 0) {
            if (key->revoked == 1)
                goto cleanup;
            if (key->expired == 1)
                goto cleanup;
            if (key->disabled == 1)
                goto cleanup;
            if (key->can_sign == 0)
                goto cleanup;

            /* FIXME: For some reason those checks above fail to
             * detect my old, unused key.  For now I'll just pick the
             * last one, which happens to be the correct one for me --
             * though this isn't very strong evidence, I have 2
             * examples... */
            gpgme_signers_clear(ctx);
            gpgme_signers_add(ctx, key);
            found_key = true;

        cleanup:
            gpgme_key_release(key);
        }
        gpgme_op_keylist_end(ctx);

        if (found_key == false) {
            fprintf(stderr, "Unable to find key that matches mail '%s'\n",
                    email.c_str());
            abort();
        }
    }
    

    gpgme_data_t gpgme_in;
    if (gpgme_data_new(&gpgme_in) != 0) {
        perror("Unable to initialize GPGME data buffer\n");
        abort();
    }
    for (const auto& line: in) {
        gpgme_data_write(gpgme_in, line.c_str(), strlen(line.c_str())-1);
        gpgme_data_write(gpgme_in, "\r\n", 2);
    }

    gpgme_data_t gpgme_out;
    if (gpgme_data_new(&gpgme_out) != 0) {
        perror("Unable to initialize GPGME data buffer\n");
        abort();
    }

    gpgme_data_seek(gpgme_in, 0, SEEK_SET);
    gpgme_data_seek(gpgme_out, 0, SEEK_SET);
    if (gpgme_op_sign(ctx, gpgme_in, gpgme_out, GPGME_SIG_MODE_CLEAR) != 0) {
        perror("Unable to GPG sign message");
        abort();
    }
    gpgme_data_seek(gpgme_in, 0, SEEK_SET);
    gpgme_data_seek(gpgme_out, 0, SEEK_SET);

    std::vector<std::string> out;
    while (true) {
        char buffer[BUFFER_SIZE];
        auto readed = gpgme_data_read(gpgme_out, buffer, BUFFER_SIZE);
        buffer[readed] = '\0';
        if (readed <= 0)
            break;

        ssize_t last_i = 0;
        for (ssize_t i = 0; i < readed; ++i) {
            if (buffer[i] == '\r' || buffer[i] == '\n') {
                buffer[i] = '\0';
                out.push_back(std::string(buffer + last_i) + "\n");
                last_i = i + 1;
            }
        }

        gpgme_data_seek(gpgme_out, readed - last_i, SEEK_CUR);
    }

    std::vector<std::string> real_out;
    bool outing = false;
    for (const auto& line: out) {
        if (strcmp("-----BEGIN PGP SIGNATURE-----\n", line.c_str()) == 0)
            outing = true;

        if (outing == true)
            real_out.push_back(line);
    }

    return real_out;
}
#endif
