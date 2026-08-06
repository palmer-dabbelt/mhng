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

logger "MHng OSX net dispatch"

if scutil <<< "show State:/Network/Global/IPv4" | grep -q "PrimaryInterface"
then
    mhng-pipe-network_up
else
    mhng-pipe-network_down
fi
