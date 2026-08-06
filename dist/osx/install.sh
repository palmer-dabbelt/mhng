#!/bin/bash
# Copyright (C) 2026 Palmer Dabbelt <palmer@dabbelt.com>
# SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause
#
# Installs the mhng LaunchAgents (daemon + network watcher) for the
# current user.  Run this instead of mhng-launch-gui-daemons on macOS.
#
# Usage: dist/osx/install.sh [PREFIX]
#   PREFIX defaults to ~/.local (matching the pconfigure default).

set -euo pipefail

prefix="${1:-$HOME/.local}"
uid="$(id -u)"
here="$(cd "$(dirname "$0")" && pwd)"
agents="$HOME/Library/LaunchAgents"

mkdir -p "$agents" "$prefix/libexec/mhng" "$HOME/.mhng"

render() {
    sed -e "s|@PREFIX@|$prefix|g" \
        -e "s|@HOME@|$HOME|g" \
        -e "s|@UID@|$uid|g" \
        "$1"
}

# The old shell-loop launcher and this LaunchAgent must not both run the
# daemon, or they'll fight over the socket.
if pgrep -x mhng-launch-gui-daemons >/dev/null 2>&1; then
    echo "Stopping legacy mhng-launch-gui-daemons..."
    killall mhng-launch-gui-daemons mhng-daemon mhng-notify 2>/dev/null || true
fi

install -m 0755 "$here/net-dispatch.sh" "$prefix/libexec/mhng/mhng-osx-net-dispatch"

for label in daemon notify netwatch; do
    plist="com.dabbelt.mhng.$label.plist"
    dst="$agents/$plist"
    render "$here/$plist" > "$dst"

    # Reload: boot out any previous copy, then bootstrap the fresh one
    # into the GUI session domain (gui/<uid>).  Bootstrapping there --
    # rather than from a random shell -- is what gives the daemon a
    # working Mach bootstrap port, and hence working getaddrinfo().
    launchctl bootout "gui/$uid/com.dabbelt.mhng.$label" 2>/dev/null || true
    launchctl bootstrap "gui/$uid" "$dst"
    echo "Loaded $plist"
done

echo
echo "Done.  Logs: ~/.mhng/daemon.log and ~/.mhng/netwatch.log"
echo "To remove: dist/osx/uninstall.sh"
