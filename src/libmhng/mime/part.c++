/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "part.h++"
#include "base64.h++"
#include <iconv.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

enum state {
    STATE_HEADERS,
    STATE_BODY,
    STATE_CHILD,
};

/* Returns TRUE if this seperates a MIME header boundary. */
static bool isterm(char c, bool space_p);

/* Filter a raw list to remove a potentially malformatted last
 * line. */
static std::vector<std::string>
filter_raw(const std::vector<std::string>& raw);

mime::part::part(const std::vector<std::string>& raw)
    : _raw(raw)
{
    enum state state = STATE_HEADERS;
    header_ptr last_header = NULL;
    std::vector<std::string> child_raw;

    /* Here's a helper function that allows the current header to be
     * committed to the MIME part under construction. */
    auto commit_header = [&](const header_ptr& header)
        {
            if (header->match("Content-Type")) {
                auto value = header->utf8();

                /* We have a boundary line, attempt to split things up
                 * accordingly. */
                if (strstr(value.c_str(), "boundary=") != NULL) {
                    char boundary[BUFFER_SIZE];
                    snprintf(boundary, BUFFER_SIZE, "%s",
                             strstr(value.c_str(), "boundary=")
                        );

                    char *bp = boundary + strlen("boundary=");
                    bool spaces = !(bp[0] == '"');
                    while (isterm(bp[0], spaces))
                        bp++;

                    char *be = bp;
                    while (!isterm(be[0], spaces))
                        be++;
                    be[0] = '\0';

                    char mime_boundary[BUFFER_SIZE];
                    snprintf(mime_boundary, BUFFER_SIZE, "--%s", bp);
                    _boundary = std::string(mime_boundary);

                    char end_boundary[BUFFER_SIZE];
                    snprintf(end_boundary, BUFFER_SIZE, "--%s--", bp);
                    _end_boundary = std::string(end_boundary);
                }

                /* Set the content-type of this message. */
                if (strstr(value.c_str(), ";") == NULL) {
                    _content_type = value;
                } else {
                    char strip[BUFFER_SIZE];
                    snprintf(strip, BUFFER_SIZE, "%s", value.c_str());
                    strstr(strip, ";")[0] = '\0';
                    _content_type = std::string(strip);
                }

                /* Checks to see if a charset was provided, and if
                 * so stores it. */
                if (strstr(value.c_str(), "charset=") != NULL) {
                    char charset[BUFFER_SIZE];
                    snprintf(charset, BUFFER_SIZE, "%s",
                             strstr(value.c_str(), "charset=")
                        );

                    char *bp = charset + strlen("charset=");
                    bool spaces = !(bp[0] == '"');
                    while (isterm(bp[0], spaces))
                        bp++;

                    char *be = bp;
                    while (!isterm(be[0], spaces))
                        be++;
                    be[0] = '\0';

                    _charset = std::string(bp);
                }

                /* Checks to see if a charset was provided, and if
                 * so stores it. */
                if (strstr(value.c_str(), "name=") != NULL) {
                    char charset[BUFFER_SIZE];
                    snprintf(charset, BUFFER_SIZE, "%s",
                             strstr(value.c_str(), "name=")
                        );

                    char *bp = charset + strlen("name=");
                    bool spaces = !(bp[0] == '"');
                    while (isterm(bp[0], spaces))
                        bp++;

                    char *be = bp;
                    while (!isterm(be[0], spaces))
                        be++;
                    be[0] = '\0';

                    _name = std::string(bp);
                }
            }

            if (header->match("Content-Transfer-Encoding"))
                _content_transfer_encoding = header->utf8();

            _headers.push_back(header);
        };

    /* Walk through every line in this MIME part and try to do
     * something with it.  Lines can either by header lines, body
     * lines, or part of the children. */
    for (const auto& raw_line: raw) {
        std::string line = raw_line;
        line.erase(line.find_last_not_of("\n\r\t")+1);

        switch (state) {
        case STATE_HEADERS:
            /* First we want to check and see if this is the last line
             * of headers, which really means we want to skip on to
             * the body. */
            if (line.size() == 0) {
                if (last_header != NULL)
                    commit_header(last_header);
                last_header = NULL;

                state = STATE_BODY;

                continue;
            }

            /* Continuation headers are also easy because all we do
             * here is append this to the last header that was
             * added. */
            if (isspace(line[0])) {
                last_header->add_line(line);
                continue;
            }

            /* It's time to start a new header. */
            if (last_header != NULL)
                commit_header(last_header);
            last_header = std::make_shared<header>(line);
            break;

        case STATE_BODY:
            /* Check if this string matches the boundary seperator,
             * which means that we need to stop parsing the body and
             * instead move on to */
            if (matches_boundary(line)) {
                state = STATE_CHILD;
                continue;
            }

            /* The remainder of the body lines simply get pushed into
             * an array for later. */
            _body_raw.push_back(raw_line);

            break;

        case STATE_CHILD:
            /* If we hit another child boundary then it's time to
             * commit this to the list of children and start parsing a
             * new child list. */
            if (matches_boundary(line)) {
                state = STATE_CHILD;
                auto child = std::make_shared<part>(filter_raw(child_raw));
                _children.push_back(child);
                child_raw = {};
                continue;
            }

            /* If this matches the end boundary for MIME parsing then
             * go back to parsing the body. */
            if (matches_end_boundary(line)) {
                state = STATE_BODY;
                auto child = std::make_shared<part>(filter_raw(child_raw));
                _children.push_back(child);
                child_raw = {};
                continue;
            }

            /* The remainder of the child body lines justs get slurped
             * up into a big array for later use. */
            child_raw.push_back(raw_line);

            break;
        }
    }

    /* Here's a special case: some messages need to be parsed
     * again! */
    if (matches_content_type("message/rfc822")) {
        auto child = std::make_shared<part>(_body_raw);
        _children.push_back(child);
    }
}


std::vector<mime::header_ptr> mime::part::headers(const std::string& name) const
{
    std::vector<header_ptr> out;

    for (const auto& header: this->headers())
        if (strcasecmp(name.c_str(), header->key().c_str()))
            out.push_back(header);

    return out;
}

std::vector<std::string> mime::part::utf8(void) const
{
    std::vector<std::string> out;

    /* If the charset is encoded then attempt to decode it into UTF-8
     * before trying to undo the HTML encoding. */
    if (_charset.known() == true) {
        std::string charset = _charset.data();

        /* Krste sends some malformed messages (UTF-8 marked as
         * US-ASCII), but because US-ASCII is a subset of UTF-8 it's
         * always safe to just directly drop the decoded message
         * out. */
        if (strcasecmp(charset.c_str(), "us-ascii") == 0)
            return decoded();

        iconv_t icd = iconv_open("UTF-8", charset.c_str());
        if (icd == NULL) {
            fprintf(stderr, "Unable to decode charset '%s'\n",
                    _charset.data().c_str());
            abort();
        }

        for (const auto& line: decoded()) {
            size_t utf_len = strlen(line.c_str()) * 4 + 1;
            char *utf = new char[utf_len + 1];
            char *utf_orig = utf;

            char *cline = strdup(line.c_str());
            char *cline_orig = cline;
            size_t cline_len = strlen(cline);

            {
                size_t err = iconv(icd,
                                   &cline,
                                   &cline_len,
                                   &utf,
                                   &utf_len);
                if ((ssize_t)err == -1) {
                    perror("Unable to decode line in body");
                    fprintf(stderr, "  line '%s'\n", line.c_str());
                    fprintf(stderr, "  charset: '%s'\n", _charset.data().c_str());
                    fprintf(stderr, "  maps to: '%s'\n", charset.c_str());
                    abort();
                }
            }
            utf[0] = '\0';

            out.push_back(utf_orig);
            free(cline_orig);
            delete[] utf_orig;
        }

        iconv_close(icd);
    } else {
        out = decoded();
    }

    /* Check to see if we have HTML data, which means we've got to
     * then go ahead and decode the HTML. */
    if (matches_content_type("text/html") == true) {
        char outfile[BUFFER_SIZE], infile[BUFFER_SIZE];
        snprintf(outfile, BUFFER_SIZE, "/tmp/mhng-html-XXXXXX");
        snprintf(infile,  BUFFER_SIZE, "/tmp/mhng-html-XXXXXX");

        int outfd = mkstemp(outfile);
        int infd  = mkstemp(infile);

        for (const auto& line: out) {
            size_t written = 0;
            while (written < line.size()) {
                ssize_t wout = ::write(outfd,
                                       line.c_str() + written,
                                       line.size() - written
                    );

                if (wout <= 0) {
                    fprintf(stderr, "Unable to write HTML file\n");
                    unlink(outfile);
                    unlink(infile);
                    return out;
                }

                written += wout;
            }

            if (::write(outfd, "\n", 1) != 1) {
                fprintf(stderr, "write of 1 failed\n");
                abort();
            }
        }

        close(outfd);
        close(infd);

        {
            char cmd[BUFFER_SIZE*3];
            snprintf(cmd, BUFFER_SIZE*3,
                     "lynx -force-html -dump -stdin < %s > %s",
                     outfile,
                     infile
                );

            if (system(cmd) != 0) {
                fprintf(stderr, "Unable to convert HTML with lynx\n");
                unlink(outfile);
                unlink(infile);
                return out;
            }
        }

        {
            std::vector<std::string> dec;

            auto file = fopen(infile, "r");
            char line[BUFFER_SIZE];
            while (fgets(line, BUFFER_SIZE, file) != NULL) {
                std::string l = line;
                l.erase(l.find_last_not_of(" \n\r\t")+1);
                dec.push_back(l);
            }
            fclose(file);

            unlink(outfile);
            unlink(infile);

            return dec;
        }
    }

    return out;
}

std::vector<std::string> mime::part::decoded(void) const
{
    std::vector<std::string> out;

    /* I handle base64 very simply: I just decode it, break newlines,
     * and store it out. */
    if (matches_encoding("base64") == true) {
        std::string last_line = "";
        for (const auto& linestr: _body_raw) {
            auto decoded_bytes = base64_decode(linestr);
            auto decoded = base64_array2string(decoded_bytes);

            for (size_t i = 0; i < decoded.size(); ++i) {
                if (decoded[i] == '\n') {
                    out.push_back(last_line);
                    last_line = "";
                } else {
                    last_line += decoded[i];
                }
            }
        }

        out.push_back(last_line);
        return out;
    }

    /* Quoted-printable has a bit of magic related to '=', but not
     * much else to do.  Note that I do merge together long line
     * fragments, so they'll have to be split up later when it's time
     * to actually display them.  In other words, this gives you the
     * raw message as UTF-8 with no filters applied. */
    if (matches_encoding("quoted-printable") == true) {
        std::string last_line = "";
        for (const auto& linestr: _body_raw) {
            std::string line = linestr;
            line.erase(line.find_last_not_of(" \n\r\t")+1);

            char linebuf[BUFFER_SIZE];
            memset(linebuf, '\0', BUFFER_SIZE);

            size_t ii = 0;
            size_t io = 0;
            while (ii < line.size()) {
                if (io >= BUFFER_SIZE - 1) {
                    fprintf(stderr, "Long line\n");
                    abort();
                }

                if (line[ii] == '=') {
                    if (line[ii+1] == '\0') {
                        ii++;
                    } else {
                        char ci1 = tolower(line[ii+1]);
                        char ci2 = tolower(line[ii+2]);

                        char str[] = {ci1, ci2, '\0'};

                        char co = strtol(str, NULL, 16);
                        if (co == '\0')
                            co = ' ';
                        linebuf[io] = co;
                        io++;
                        ii += 3;
                    }
                } else {
                    linebuf[io] = line[ii];
                    io++;
                    ii++;
                }
            }

            last_line = last_line + std::string(linebuf);

            if (line[line.size()-1] != '=') {
                out.push_back(last_line);
                last_line = "";
            }
        }

        out.push_back(last_line);
        return out;
    }

    /* The default is to just strip whitespace at the end of the
     * line and do nothing else! */
    for (const auto& line: _body_raw) {
        std::string buffer = line;
        buffer.erase(line.find_last_not_of(" \n\r\t")+1);
        out.push_back(buffer);
    }
    return out;
}

void mime::part::write(FILE *out) const
{

    if (matches_encoding("base64") == true) {
        /* I handle base64 very simply: I just decode it, break newlines,
         * and store it out. */
        for (const auto& linestr: _body_raw) {
            auto decoded = base64_decode(linestr);
            if (decoded.size() == 0)
                continue;

            auto written = fwrite(&decoded[0], decoded.size(), 1, out);
            if (written != 1) {
                perror("Unable to write");
                abort();
            }
        }
        return;
    } else if (matches_encoding("7bit")) {
        /* 7bit clean can just be written directly. */
        for (const auto& decoded: _body_raw) {
            if (decoded.size() == 0)
                continue;

            auto written = fwrite(&decoded[0], decoded.size(), 1, out);
            if (written != 1) {
                perror("Unable to write");
                abort();
            }
        }
    } else {
        fprintf(stderr, "Unable to decode MIME part for writing\n");
        fprintf(stderr, "  Content-Transfer-Encoding: %s\n", encoding().c_str());
        abort();
    }
}

mime::part_ptr mime::part::body(void) const
{
    /* If there isn't a MIME type then this isn't a MIME message at
     * all! */
    if (_content_type.known() == false)
        return NULL;

    /* If this doesn't have any children then there's nothing we can
     * do to search it. */
    if (_children.size() == 0)
        return NULL;

    /* RFC-822 messages are actually just containers for other
     * messages, so we simply pass this call on. */
    if (matches_content_type("message/rfc822") == true)
        return _children[0]->body();

    /* Attempt to select the correct multipart, alternative means we
     * pick one of them. */
    if ((matches_content_type("multipart/alternative") == true)
        || (matches_content_type("multipart/related") == true)
        || (matches_content_type("multipart/signed") == true)) {
        /* If there's anything that matches multipart-signed then show
         * its child. */
        for (const auto& child: _children) {
            if (child->matches_content_type("multipart/signed")) {
                auto cbody = child->body();
                if (cbody != NULL)
                    return cbody;
            }
        }

        /* First, prefer anything that's just plain text. */
        for (const auto& child: _children) {
            if (child->matches_content_type("text/plain")) {
                /* Check for attachments, they shouldn't be shown
                 * inline. */
                bool allowed = true;
                for (const auto& hdr: child->headers("Content-Disposition")) {
                    auto v = hdr->single_line();
                    auto a = std::string("attachment");
                    if (strncasecmp(v.c_str(), a.c_str(), a.size()) == 0)
                        allowed = false;
                }

                if (allowed == true)
                    return child;
            }
        }

        /* Attempt a recursive parse, looking for a plain text part.
         * The idea here is that it might be hidden in another
         * "multipart/related" or "multipart/mixed" sort of thing. */
        for (const auto& child: _children) {
            auto child_body = child->body();
            if (child_body != NULL)
                return child_body;
        }

        /* Finally, if there's a HTML part then show it begrudgingly. */
        for (const auto& child: _children)
            if (child->matches_content_type("text/html"))
                return child;

        /* If there's nothing that we know how to deal with then just
         * go ahead and dump out everything. */
        return NULL;
    }

    /* If we've got a "mixed" multipart then we actually need to
     * return _all_ of them! */
    if (matches_content_type("multipart/mixed") == true) {
        auto out_raw = std::vector<std::string>();

        bool first_child = true;
        for (const auto& child: _children) {
            /* The first child will have its headers set, everything
             * else won't. */
            if (first_child == true) {
                for (const auto& raw: child->_raw) {
                    if (strlen(raw.c_str()) <= 2)
                        break;

                    out_raw.push_back(raw);
                }

                /* Here we push a content-type in because this has
                 * already been decoded. */
                out_raw.push_back("Content-Type: text/plain; charset=UTF-8\r\n");
                out_raw.push_back("Content-Transfer-Encoding: 8bit\r\n");
                out_raw.push_back("\r\n");
            }

            /* Recursively search every other message part for a body
             * (which may very well just be the full contents of the
             * message), which itself gets CATted into the body of the
             * message. */
            auto child_body = child->body();
            if (child_body == NULL)
                child_body = child;

            if (child_body->matches_content_type("text/plain") == true)
                for (const auto& line: child_body->utf8())
                    out_raw.push_back(line + "\r\n");

            if (child->matches_content_type("text/html") == true)
                for (const auto& line: child_body->utf8())
                    out_raw.push_back(line + "\r\n");

            first_child = false;
        }

        return std::make_shared<part>(out_raw);
    }

    /* Otherwise we can't find anything, so just give up. */
    return NULL;
}

mime::part_ptr mime::part::signature(void) const
{
    /* If there isn't a MIME type then this isn't a MIME message at
     * all! */
    if (_content_type.known() == false)
        return NULL;

    /* Multipart-alternative sections should be checked. */
    if (matches_content_type("multipart/alternative")) {
        for (const auto& child: _children) {
            auto csig = child->signature();
            if (csig != NULL)
                return csig;
        }
    }

    if (matches_content_type("multipart/signed") == false)
        return NULL;

    for (const auto& child: _children)
        if (child->matches_content_type("application/pgp-signature") == true)
            return child;

    return NULL;
}

void mime::part::add_header(const header_ptr& header)
{
    _headers.push_back(header);

    std::vector<std::string> new_raw;
    bool added = false;
    for (const auto& raw: _raw) {
        if (added == false && (strlen(raw.c_str()) == 1)) {
            added = true;
            for (const auto& hraw: header->raw())
                new_raw.push_back(hraw + "\n");
        }

        new_raw.push_back(raw);
    }

    if (added == false) {
        fprintf(stderr, "No body seperator?\n");
        abort();
    }

    _raw = new_raw;
}

void mime::part::add_header(const std::string& key,
                            const std::string& value)
{
    auto header = std::make_shared<mime::header>(key + ": " + value);
    add_header(header);
}

bool mime::part::matches_boundary(const std::string& line) const
{
    if (_boundary.known() == false)
        return false;

    auto boundary = _boundary.data();
    if (strcmp(line.c_str(), boundary.c_str()) == 0)
        return true;

    return false;
}

bool mime::part::matches_end_boundary(const std::string& line) const
{
    if (_end_boundary.known() == false)
        return false;

    auto boundary = _end_boundary.data();
    if (strcmp(line.c_str(), boundary.c_str()) == 0)
        return true;

    return false;
}

bool mime::part::matches_content_type(const std::string& type) const
{
    if (_content_type.known() == false)
        return false;

    auto boundary = _content_type.data();
    if (strcasecmp(type.c_str(), boundary.c_str()) == 0)
        return true;

    return false;
}

bool mime::part::matches_encoding(const std::string& type) const
{
    if (_content_transfer_encoding.known() == false)
        return false;

    auto boundary = _content_transfer_encoding.data();
    if (strcasecmp(type.c_str(), boundary.c_str()) == 0)
        return true;

    return false;
}

std::string mime::part::encoding(void) const
{
    if (_content_transfer_encoding.known() == false)
        return "((unknown))";

    return _content_transfer_encoding.data();
}

bool isterm(char c, bool space_p)
{
    if (isspace(c))
        return space_p;

    if (c == '"')
        return true;

    if (c == ';')
        return true;

    if (c == '\0')
        return true;

    return false;
}

std::vector<std::string>
filter_raw(const std::vector<std::string>& raw)
{
    auto size = raw.size();
    if (size == 0)
        return raw;

    auto last = raw[size-1];
    if (strcmp(last.c_str(), "\n") == 0) {
        return std::vector<std::string>(raw.begin(),
                                        raw.begin() + size - 1
            );
    }

    return raw;
}
