/* Copyright (C) 2013-2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifdef HAVE_GPGME
#include "gpg_sign.h++"
#include <gpgme.h>
#include <string.h>
#include <stdlib.h>

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
        char buffer[BUFFER_SIZE+1];
        ssize_t readed = gpgme_data_read(gpgme_out, buffer, BUFFER_SIZE);

        if (readed <= 0)
            break;

        buffer[readed] = '\0';

        ssize_t last_i = 0;
        for (ssize_t i = 0; i < readed; ++i) {
            if (buffer[i] == '\r' || buffer[i] == '\n') {
                buffer[i] = '\0';
                out.push_back(std::string(buffer + last_i) + "\n");
                last_i = i + 1;
            }
        }

        gpgme_data_seek(gpgme_out, last_i - readed, SEEK_CUR);
    }

    std::vector<std::string> real_out;
    bool outing = false;
    bool ended = false;
    for (const auto& line: out) {
        if (strcmp("-----BEGIN PGP SIGNATURE-----\n", line.c_str()) == 0)
            outing = true;

        if (outing == true)
            real_out.push_back(line);

        if (strcmp("-----END PGP SIGNATURE-----\n", line.c_str()) == 0) {
            ended = outing;
            break;
        }
    }

    /* Check to make sure we've got a valid PGP output block, and
     * attempt to either fix it up or provide some debugging
     * information. */
    if (outing == false) {
        fprintf(stderr, "Unable to match PGP Signature Beginnig\n");
        fprintf(stderr, "gpgme_op_sign output:\n");
        for (const auto& line: out)
            fprintf(stderr, "  %s", line.c_str());
    }

    if (ended == false)
        real_out.push_back("-----END PGP SIGNATURE-----\n");

    return real_out;
}


mhng::gpg_verification mhng::gpg_verify(const std::vector<std::string>& msg,
                                        const std::vector<std::string>& sig,
                                        std::string email __attribute__((unused)))
{
    gpgme_error_t err;
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

    gpgme_data_t gpgme_msg;
    if (gpgme_data_new(&gpgme_msg) != 0) {
        perror("Unable to initialize GPGME data buffer\n");
        abort();
    }
    for (const auto& line: msg) {
        gpgme_data_write(gpgme_msg, line.c_str(), strlen(line.c_str())-1);
        gpgme_data_write(gpgme_msg, "\r\n", 2);
    }

    gpgme_data_t gpgme_sig;
    if (gpgme_data_new(&gpgme_sig) != 0) {
        perror("Unable to initialize GPGME data buffer\n");
        abort();
    }
    for (const auto& line: sig) {
        gpgme_data_write(gpgme_sig, line.c_str(), strlen(line.c_str()));
        gpgme_data_write(gpgme_sig, "\r\n", 2);
    }

    gpgme_data_seek(gpgme_msg, 0, SEEK_SET);
    gpgme_data_seek(gpgme_sig, 0, SEEK_SET);

    if ((err = gpgme_op_verify(ctx, gpgme_sig, gpgme_msg, NULL)) != 0) {
        fprintf(stderr, "WARNING: Unable to run verification\n");
        fprintf(stderr, "         GPGme Error: '%s'\n", gpgme_strerror(err));
        return mhng::gpg_verification::ERROR;
    }

    auto verres = gpgme_op_verify_result(ctx);
    for (auto sig = verres->signatures; sig != NULL; sig = sig->next) {
        switch (sig->summary) {
        case GPGME_SIGSUM_VALID:
        case GPGME_SIGSUM_GREEN:
            return mhng::gpg_verification::SUCCESS;
            break;

        case GPGME_SIGSUM_RED:
            fprintf(stderr, "WARNING: Verification Failed\n");
            return mhng::gpg_verification::FAIL;

        case GPGME_SIGSUM_KEY_REVOKED:
        case GPGME_SIGSUM_KEY_EXPIRED:
        case GPGME_SIGSUM_SIG_EXPIRED:
        case GPGME_SIGSUM_KEY_MISSING:
        case GPGME_SIGSUM_CRL_MISSING:
        case GPGME_SIGSUM_CRL_TOO_OLD:
        case GPGME_SIGSUM_BAD_POLICY:
        case GPGME_SIGSUM_SYS_ERROR:
            fprintf(stderr, "WARNING: Unable to run verification\n");
            return mhng::gpg_verification::ERROR;
            break;
        }
    }

    return mhng::gpg_verification::ERROR;
}
#endif
