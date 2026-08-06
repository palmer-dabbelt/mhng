# macOS daemon / network integration

This directory is the macOS counterpart to `dist/nm-dispatch.sh` (the
Linux NetworkManager hook) and to `mhng-launch-gui-daemons`.

## Why LaunchAgents

`mhng-daemon` fork()+exec()s the IMAP helpers (`mhimap-idle`,
`mhimap-sync`, ...) for every connection. On macOS `getaddrinfo()`
doesn't resolve names itself -- it asks `mDNSResponder` over XPC, found
via the process's **Mach bootstrap port**. That port is inherited across
both `fork()` *and* `exec()`, so a fresh IMAP process does **not** get a
fresh resolver.

If the daemon is started from a session detached from the GUI login
(over SSH, in a `tmux`/`screen` started before login, as a
`LaunchDaemon`, ...), every IMAP child inherits a bootstrap port that
can't reach `mDNSResponder`, and `getaddrinfo("imap.gmail.com", ...)`
fails forever with

    getaddrinfo: 'nodename nor servname provided, or not known'

even though the network is fine. Running the daemon as a per-user
**LaunchAgent** fixes this: launchd bootstraps it into the `gui/<uid>`
domain, so it (and its children) get a working bootstrap port.

## Install

    dist/osx/install.sh [PREFIX]     # PREFIX defaults to ~/.local

This installs three LaunchAgents into `~/Library/LaunchAgents`:

- `com.dabbelt.mhng.daemon`   -- runs `mhng-daemon` (replaces the
  `while true; mhng-daemon` loop in `mhng-launch-gui-daemons`).
- `com.dabbelt.mhng.notify`   -- runs `mhng-notify` (replaces the
  `while true; mhng-notify` loop). On a build without libnotify this
  just exits immediately, which is harmless.
- `com.dabbelt.mhng.netwatch` -- runs `net-dispatch.sh` on every network
  change, telling the daemon to go up/down.

To remove:

    dist/osx/uninstall.sh

## How the network watcher works

There's no dispatcher-script mechanism on macOS, so instead of hooking a
daemon we let launchd's `WatchPaths` notice that `configd` rewrote the
resolver/SystemConfiguration files on a network change, and relaunch
`net-dispatch.sh`. That script checks for a primary IPv4 interface with
`scutil` and sends `net_up`/`net_down` to the daemon via
`mhng-pipe-network_up` / `mhng-pipe-network_down` -- the same messages
the Linux hook sends. A `net_up` makes the daemon tear down and
reconnect its IMAP children, which is what recovers a sleep/wake or a
network switch.
