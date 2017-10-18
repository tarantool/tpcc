#!/usr/bin/env bash

# Build Tarantool
cd /tarantool
git reset --hard 1b37e331813608b12fc8e13f6810c95c9873c5d9
cmake . -DENABLE_DIST=ON; make; make install

# Build tarantool-c
cd /tarantool-c
git pull
cd third_party/msgpuck/; cmake . ; make; make install; cd ../..
cmake . ; make; make install

# Build tpcc
cd ${PATH_TO_TPCC}/src; make

# Run Tarantool
cd ${PATH_TO_TPCC}
tarantoolctl start start-server.lua
echo "waiting load snapshot to tarantool... 180s"; sleep 180
cat /usr/local/var/log/tarantool/start-server.log

# Run SysBench, Print results to screen, Save results to result.txt
apt-get install -y -f gdb
cd ${PATH_TO_TPCC}
./tpcc_start -w15 -r10 -l60  | tee temp-result.txt
cat temp-result.txt | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee result.txt

tarantoolctl stop start-server.lua