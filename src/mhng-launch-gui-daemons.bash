#!/bin/bash

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
