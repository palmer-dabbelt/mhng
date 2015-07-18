#ifndef LIBMHNG__MIME__BASE64_HXX
#define LIBMHNG__MIME__BASE64_HXX

#include <string>
#include <vector>

std::string base64_encode(unsigned char const* bytes, unsigned int len);
std::string base64_decode(std::string const& s);

/* Decodes a base64 string into a fixed buffer, which must be of
 * sufficient size.  Note that this doesn't do anything like
 * NUL-termination because it expects binary data. */
ssize_t base64_decode(const std::string& in, unsigned char *out);
ssize_t base64_decode(const std::string& in, char *out);

/* Encodes a whole file as base64, returning each line as a string
 * without a newline. */
std::vector<std::string> base64_encode_file(const std::string& filename);

#endif
