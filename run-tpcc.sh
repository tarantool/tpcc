#!/usr/bin/env bash

apt-get update && apt-get install -y -f ssh vim git dh-autoreconf \
      pkg-config build-essential cmake coreutils sed libreadline-dev \
      libncurses5-dev libyaml-dev libssl-dev libcurl4-openssl-dev \
      libunwind-dev python python-pip python-setuptools python-dev \
      python-msgpack python-yaml python-argparse python-six python-gevent

# Build Tarantool
git clone --recursive https://github.com/tarantool/tarantool.git -b 1.8 tarantool
cd tarantool; cmake . -DENABLE_DIST=ON ; make; make install; cd ..;

# Build tarantool-c
git clone --recursive https://github.com/tarantool/tarantool-c tarantool-c
cd tarantool-c/third_party/msgpuck/; cmake . ; make; make install; cd ../../..;
cd tarantool-c; cmake . ; make; make install; cd ..;

# Build msgpack
git clone https://github.com/msgpack/msgpack-c.git
cd msgpack-c ; cmake . ; make ; make install ; cd ..

# Build tpcc
cd src ; make ; cd ..

# Run Tarantool
tarantool create_table.lua
tarantool start-server.lua > /dev/null &
sleep 2; TNT_PID=$!

# Run SysBench, Print results to screen, Save results to result.txt
apt-get install -y -f gdb
./tpcc_load -w 5
echo "test_name:result[TpmC]"
echo -n "tpcc:"
./tpcc_start -w5 -r10 -l300  | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee result.txt

# Clear
kill $TNT_PID ; rm -f *.xlog ; rm -f *.snap