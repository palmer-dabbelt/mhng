#!/bin/bash
# Copyright (C) 2026 Palmer Dabbelt <palmer@dabbelt.com>
# SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause
#
# Removes the mhng LaunchAgents installed by install.sh.

set -uo pipefail

uid="$(id -u)"
agents="$HOME/Library/LaunchAgents"

for label in daemon notify netwatch; do
    launchctl bootout "gui/$uid/com.dabbelt.mhng.$label" 2>/dev/null || true
    rm -f "$agents/com.dabbelt.mhng.$label.plist"
    echo "Removed com.dabbelt.mhng.$label"
done
