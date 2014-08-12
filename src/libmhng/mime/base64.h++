#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

/* Decodes a base64 string into a fixed buffer, which must be of
 * sufficient size.  Note that this doesn't do anything like
 * NUL-termination because it expects binary data. */
ssize_t base64_decode(const std::string& in, unsigned char *out);
