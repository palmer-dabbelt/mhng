#!/bin/bash

trap true TERM
killall mhng-launch-gui-daemons
killall mhng-daemon
killall mhng-notify
trap - TERM

# mhng-daemon and mhng-notify now open (and rotate) their own logs --
# ~/.mhng/daemon.log and ~/.mhng/notify.log respectively -- so there's
# no shell redirect here anymore.  See mhng::logfile.
( while true
do
    mhng-daemon
    sleep 10s
done ) &

( while true
do
    mhng-notify
    sleep 10s
done ) &

wait
