#include "harness_start.bash"

# line_buffer::get() writes a '\0' after however much of the line it
# copied, and used to clamp the copy to exactly the caller's buffer
# size -- so that '\0' landed one byte past the end of it.  Every
# caller in client.c++ hands it a `char buffer[BUFFER_SIZE]` along
# with BUFFER_SIZE, and IMAP lines do get that long: a References:
# header with a few dozen message IDs in it, or a URL in a body.
#
# The "canary" here is a byte sitting immediately after the part of
# the buffer get() is allowed to touch, and it reads CLOBBERED on the
# lines that exactly fill or overrun the buffer if the clamp is off by
# one again.  The 64-byte buffer is the same shape as the 1024-byte
# one in client.c++, just small enough to write a test around.
$PTEST_BINARY line-buffer > out.test

cat >out.gold <<"EOF"
line of 1: returned 3, copied 1, canary intact
line of 63: returned 65, copied 63, canary intact
line of 64: returned 66, copied 63, canary intact
line of 65: returned 67, copied 63, canary intact
line of 200: returned 202, copied 63, canary intact
EOF

#include "harness_end.bash"
