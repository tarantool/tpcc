#!/bin/bash

if [ ! -d "$tarantool" ]; then
	git clone --recursive https://github.com/tarantool/tarantool.git -b 1.8 tarantool
fi

cd tarantool; git pull; cmake .; make; cd ..;

cd src ; make ; cd .. ;

./tarantool/src/tarantool create_table.lua

./tarantool/src/tarantool start-server.lua &
sleep 2
TNT_PID=$!

./tpcc_load -h127.0.0.1 -w 5
echo -n "tpcc:"
./tpcc_start -h127.0.0.1 -w5 -c1 -r10 -l300  | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee result.txt

kill $TNT_PID
wait
rm *.xlog
rm *.snap

python export.py auth.conf result.txt
