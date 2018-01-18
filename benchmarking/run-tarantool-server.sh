#!/usr/bin/env bash

tarantool tpcc-server.lua

STATUS="$(echo box.info.status | tarantoolctl connect /tmp/tpcc-server.sock | grep -e "- running")"

while [ ${#STATUS} -eq "0" ]; do
    echo "waiting load snapshot to tarantool..."
    sleep 5
    STATUS="$(echo box.info.status | tarantoolctl connect /tmp/tpcc-server.sock | grep -e "- running")"
done
cat ./tpcc-server.log
