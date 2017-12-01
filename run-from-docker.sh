#!/usr/bin/env bash

if [ ! -n "${PATH_TO_TPCC}" ]; then PATH_TO_TPCC=$(pwd); fi

# Build Tarantool
cd /tarantool
git pull
if [ ! -n "${BRANCH}" ]; then BRANCH="1.8"; fi
git checkout ${BRANCH};
cmake . -DENABLE_DIST=ON; make; make install

# define tarantool version
cd ${PATH_TO_TPCC}
TAR_VER=$(tarantool -v | grep -e "Tarantool" |  grep -oP '\s\K\S*')
echo ${TAR_VER} | tee version.txt

# Build tarantool-c
cd /tarantool-c
git pull
cd third_party/msgpuck/; cmake . ; make; make install; cd ../..
cmake . ; make; make install

# Build tpcc
cd ${PATH_TO_TPCC}/src; make

# Run Tarantool
cd ${PATH_TO_TPCC}
./run-tarantool-server.sh

# Run tpcc
apt-get install -y -f gdb
cd ${PATH_TO_TPCC}
TIME=${TIME} ./run-test.sh
