#!/bin/bash

interface="$1"
event="$2"

logger "MHng NM dispatch"

if [[ "$(nmcli d | grep connected | grep -v disconnected | wc -l)" == 0 ]]
then
    su palmerdabbelt -c ~palmerdabbelt/.local/bin/mhng-pipe-network_down
else
    su palmerdabbelt -c ~palmerdabbelt/.local/bin/mhng-pipe-network_up
fi
