#!/bin/bash

killall mhng-launch-gui-daemons
killall mhng-daemon
killall mhng-notify

( while true
do
    mhng-daemon &>> ~/.mhng/daemon.log
    sleep 10s
done ) &

( while true
do
    mhng-notify &>> ~/.mhng/notify.log
    sleep 10s
done ) &

wait
