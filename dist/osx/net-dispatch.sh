#!/bin/bash
# Copyright (C) 2026 Palmer Dabbelt <palmer@dabbelt.com>
# SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause
#
# macOS network-change hook for the mhng daemon, launched by
# com.dabbelt.mhng.netwatch.plist.  This mirrors dist/nm-dispatch.sh,
# which is wired into NetworkManager on Linux.
#
# We decide "up" by asking configd whether there's a primary IPv4
# interface -- i.e. a default route exists.  Deliberately no DNS lookup
# here: DNS being wedged is exactly the failure we're recovering from,
# so the daemon (not this script) is the thing that should discover a
# name no longer resolves.  A net_up just kicks the daemon into tearing
# down and reconnecting its IMAP children.

# Own (and rotate) our own log instead of leaning on the LaunchAgent's
# StandardErrorPath, matching what mhng-daemon/mhng-notify do via
# mhng::logfile.  This job is short-lived -- one run per network event --
# so a rotate-on-start is enough; there's no long-running process to need
# a background timer.  Same policy and env overrides as mhng::logfile:
# 8 MiB per generation, 4 generations kept.
log="$HOME/.mhng/netwatch.log"
max_bytes="${MHNG_LOG_MAX_BYTES:-8388608}"
keep="${MHNG_LOG_KEEP:-4}"

rotate_log() {
    local size
    size="$(stat -f%z "$log" 2>/dev/null || echo 0)"
    [ "$size" -lt "$max_bytes" ] && return

    if [ "$keep" -le 0 ]; then
        : > "$log"
        return
    fi

    # Drop the oldest, shift the rest down, then start fresh -- the same
    # generation shuffle mhng::logfile::rotate_if_needed() does.
    rm -f "$log.$keep"
    local i
    for (( i = keep; i > 1; i-- )); do
        [ -e "$log.$((i - 1))" ] && mv "$log.$((i - 1))" "$log.$i"
    done
    [ -e "$log" ] && mv "$log" "$log.1"
}

mkdir -p "$HOME/.mhng"
rotate_log
exec >> "$log" 2>&1
echo "=== $(date): MHng OSX net dispatch ==="

logger "MHng OSX net dispatch"

if scutil <<< "show State:/Network/Global/IPv4" | grep -q "PrimaryInterface"
then
    mhng-pipe-network_up
else
    mhng-pipe-network_down
fi
