#!/usr/bin/env bash

#apt-get update && apt-get install -y -f ssh vim git dh-autoreconf \
#      pkg-config build-essential cmake coreutils sed libreadline-dev \
#      libncurses5-dev libyaml-dev libssl-dev libcurl4-openssl-dev \
#      libunwind-dev python python-pip python-setuptools python-dev \
#      python-msgpack python-yaml python-argparse python-six python-gevent

# Build msgpack
#git clone https://github.com/msgpack/msgpack-c.git
#cd msgpack-c ; cmake . ; make ; make install ; cd ..

# Build Tarantool
git clone --recursive https://github.com/tarantool/tarantool.git -b 1.8 tarantool
cd tarantool; git reset --hard 1b37e331813608b12fc8e13f6810c95c9873c5d9 s; cmake . ; make; make install; cd ..;

# Build tarantool-c
git clone --recursive https://github.com/tarantool/tarantool-c tarantool-c
cd tarantool-c/third_party/msgpuck/; cmake . ; make; make install; cd ../../..;
cd tarantool-c; cmake . ; make; make install; cd ..;

# Build tpcc
cd src ; make ; cd ..

# Run Tarantool
cd snapshot; tarantool ../start-server.lua &
sleep 60; cd .. ; TNT_PID=$!

# Run SysBench, Print results to screen, Save results to result.txt
apt-get install -y -f gdb
#./tpcc_load -w 15
echo "test_name:result[TpmC]"
echo -n "tpcc:"
./tpcc_start -w15 -r10 -l600  | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee result.txt

kill $TNT_PID