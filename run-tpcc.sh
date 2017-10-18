#!/usr/bin/env bash

# Build Tarantool
cd ../tarantool;
git reset --hard 1b37e331813608b12fc8e13f6810c95c9873c5d9;
cmake . -DENABLE_DIST=ON; make; make install;
cd ../tpcc-tarantool;

# Build tarantool-c
cd ../tarantool-c;
git pull;
cd third_party/msgpuck/; cmake . ; make; make install; cd ../..;
cmake . ; make; make install;
cd ../tpcc-tarantool

# Build tpcc
cd src ; make ; cd ..

# Run Tarantool
tarantoolctl start start-server.lua ;
echo "waiting load snapshot to tarantool... 90s"; sleep 90;
cat /usr/local/var/log/tarantool/start-server.log

# Run SysBench, Print results to screen, Save results to result.txt
apt-get install -y -f gdb
./tpcc_start -w15 -r10 -l60  | tee temp-result.txt

echo "test_name:result[TpmC]"
echo -n "tpcc:"
cat temp-result.txt | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee result.txt

tarantoolctl stop start-server.lua