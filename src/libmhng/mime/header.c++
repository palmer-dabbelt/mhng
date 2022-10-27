/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "header.h++"
#include "base64.h++"
#include "message.h++"
#include <iconv.h>
#include <string.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 10240
#endif

static char *strstr_esc(const char *str, const char *match);

mime::header::header(const std::string& first_line)
    : _raw({first_line})
{
}

std::optional<std::string> mime::header::utf8_maybe(void) const
{
    char line[BUFFER_SIZE];
    snprintf(line, BUFFER_SIZE, "%s", single_line().c_str());

    char out[BUFFER_SIZE * 4 + 1];

    size_t i = 0, oi = 0;
    while (i < strlen(line)) {
        /* Check to see if we've got something that should be decoded
         * as a special charset, otherwise just copy over the
         * character. */
        if (strncmp(line + i, "=?", 2) == 0) {
            char charset[BUFFER_SIZE];
            snprintf(charset, BUFFER_SIZE, "%s", line + i + 2);
            if (strstr(charset, "?") == NULL) {
                /* We might just have a "=?" in a regular message, in which
                 * case we shouldn't be treating this as a special character.
                 * and should instead just pass through the character. */
                goto actually_not_special;
            }
            strstr(charset, "?")[0] = '\0';

            char raw[BUFFER_SIZE];
            size_t out_offset = 0;
            if (strncmp(line + i + strlen(charset) + 2, "?B?", 3) == 0) {
                char base64[BUFFER_SIZE];
                snprintf(base64, BUFFER_SIZE,
                         "%s", line + i + strlen(charset) + 5);

                if (strstr(base64, "?=") == NULL) {
                    fprintf(stderr, "Unable to terminate base64 section\n");
                    fprintf(stderr, "  line: '%s'\n", line + i);
                    abort();
                }

                /* base64_decode() expects a newline at the end of the input
                 * string, and it's easier to just whack it in there than to
                 * change the mime part parser.. */
                auto end = strstr(base64, "?=");
                end[0] = '\n';
                end[1] = '\0';

                auto dec = base64_decode(base64);
                auto dec_str = base64_array2string(dec);
                strcpy(raw, dec_str.c_str());
                out_offset = strlen(base64);
            } else if (strncmp(line + i + strlen(charset) + 2, "?Q?", 3) == 0) {
                char qp[BUFFER_SIZE];
                snprintf(qp, BUFFER_SIZE, "%s", line + i + strlen(charset) + 5);
                if (strstr(qp, "?=") == NULL) {
                    fprintf(stderr, "Unable to terminate QP section\n");
                    fprintf(stderr, "  line: '%s'\n", line + i);
                    return {};
                }
                strstr(qp, "?=")[0] = '\0';

                size_t qi = 0, ri = 0;
                while (qi < strlen(qp)) {
                    if (qp[qi] == '=') {
                        char hex[3];
                        hex[0] = qp[qi + 1];
                        hex[1] = qp[qi + 2];
                        hex[2] = '\0';

                        raw[ri] = strtol(hex, NULL, 16);
                        ri++;
                        qi += 3;
                    } else if (qp[qi] == '_') {
                        raw[ri] = ' ';
                        ri++;
                        qi++;
                    } else {
                        raw[ri] = qp[qi];
                        ri++;
                        qi++;
                    }
                }
                raw[ri] = '\0';

                out_offset = strlen(qp);
            }

            if (strcmp(charset, "ks_c_5601-1987") == 0)
                strcpy(charset, "EUC-KR");

            iconv_t icd = iconv_open("UTF-8", charset);

            char *raw_p = raw;
            size_t raw_l = strlen(raw_p);
            char utf[BUFFER_SIZE * 4 + 1];
            char *utf_p = utf;
            size_t utf_l = BUFFER_SIZE * 4;

            size_t err = iconv(icd,
                               &raw_p,
                               &raw_l,
                               &utf_p,
                               &utf_l);
            if ((ssize_t)err == -1) {
                perror("Unable to decode line in header");
                fprintf(stderr, "  line '%s'\n", raw);
                fprintf(stderr, "  charset: '%s'\n", charset);
                iconv_close(icd);
                return {};
            }

            *utf_p = '\0';

            iconv_close(icd);

            strcpy(out + oi, utf);
            oi += strlen(utf);
            out[oi] = '\0';
            i += 2;
            i += strlen(charset);
            i += 3;
            i += out_offset;
            i += 2;

            /* FIXME: Is this correct?  I can't tell if I'm supposed
             * to do this or if Google is */
            if (isspace(line[i]))
                i++;
        } else {
        actually_not_special:
            out[oi] = line[i];
            oi++;
            i++;
        }
    }

    out[oi] = '\0';
    return out;
}

std::string mime::header::single_line(void) const
{
    std::string out = "";

    for (size_t i = 0; i < _raw.size(); ++i) {
        /* Usually we just want to strip spaces from the raw line, but
         * if it's the first line then we also need to strip the key
         * string. */
        auto cont = _raw[i];
        if (i == 0) {
            char buffer[BUFFER_SIZE];
            snprintf(buffer, BUFFER_SIZE, "%s", cont.c_str());
            cont = strstr(buffer, ":") + 1;
        }

        /* Now actually strip the spaces from the front. */
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%s", cont.c_str());

        char *bufp = buffer;
        while (isspace(bufp[0]))
            bufp++;

        if (strlen(out.c_str()) == 0)
            out = bufp;
        else
            out = out + " " + bufp;
    }

    return out;
}

std::vector<std::string> mime::header::split_commas(void) const
{
    std::vector<std::string> out;

    auto line = utf8();
    const char *start = line.c_str();
    while (strstr_esc(start, ",") != NULL) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%s", start);

        char *end = strstr_esc(buffer, ",");
        *end = '\0';
        while ((end > buffer) && isspace(end[-1])) {
            end--;
            *end = '\0';
        }

        out.push_back(buffer);

        start = strstr_esc(start, ",") + 1;
        while (isspace(*start))
            start++;
    }
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", start);
    char *end = buffer + strlen(buffer);
    while ((end > buffer) && isspace(end[-1])) {
        end--;
        *end = '\0';
    }
    out.push_back(buffer);

    return out;
}

std::string mime::header::key(void) const
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", _raw[0].c_str());
    strstr(buffer, ":")[0] = '\0';
    return buffer;
}

std::string mime::header::key_downcase(void) const
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", _raw[0].c_str());
    strstr(buffer, ":")[0] = '\0';

    for (size_t i = 0; i < strlen(buffer); ++i)
        buffer[i] = tolower(buffer[i]);

    return buffer;
}

void mime::header::add_line(const std::string& line)
{
    _raw.push_back(line);
}

bool mime::header::match(const std::string& key) const
{
    if (strncasecmp(key.c_str(), _raw[0].c_str(), strlen(key.c_str())) != 0)
        return false;

    if (_raw[0].c_str()[strlen(key.c_str())] != ':')
        return false;

    return true;
}

bool mime::header::match(const std::vector<std::string>& keys) const
{
    for (const auto& key: keys)
        if (match(key))
            return true;

    return false;
}

char *strstr_esc(const char *str, const char *match)
{
    bool escape = false;
    bool quote = false;
    for (size_t i = 0; i < strlen(str); ++i) {
        if (!escape && str[i] == '\\')
            escape = true;
        else if (!escape && str[i] == '"')
            quote = !quote;

        if (quote) {
            escape = false;
            continue;
        }

        if (escape) {
            escape = false;
            continue;
        }

        if (strncmp(str + i, match, strlen(match)) == 0)
            return (char *)(str + i);
    }

    return NULL;
}
