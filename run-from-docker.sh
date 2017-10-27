#!/usr/bin/env bash

# Build Tarantool
cd /tarantool
git pull
if [ -n "${BRANCH}" ]; then git checkout ${BRANCH}; fi
branch_name="$(git symbolic-ref HEAD 2>/dev/null)"
branch_name=${branch_name##refs/heads/}
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
tarantool tpcc-server.lua

STATUS="$(echo box.info.status | tarantoolctl connect /usr/local/var/run/tarantool/tpcc-server.control | grep -e "- running")"
while [ ${#STATUS} -eq "0" ]; do
    echo "waiting load snapshot to tarantool..."
    sleep 5
    STATUS="$(echo box.info.status | tarantoolctl connect /usr/local/var/run/tarantool/tpcc-server.control | grep -e "- running")"
done

cat /usr/local/var/log/tarantool/tpcc-server.log

# Run SysBench, Print results to screen, Save results to result.txt
echo "---------------------------------------------"
TAR_VER=$(tarantool -v | grep -e "Tarantool" |  grep -oP '\s\K\S*')
echo $TAR_VER"["$branch_name"]" | tee version.txt
echo "---------------------------------------------"

if [ ! -n "${TIME}" ]; then TIME=2400; fi

apt-get install -y -f gdb
./tpcc_start -w15 -r10 -l${TIME} -i60 | tee temp-result.txt
cat temp-result.txt | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee result.txt
