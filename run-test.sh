#!/usr/bin/env bash

if [ ! -n "${TIME}" ]; then TIME=2400; fi
if [ ! -n "${RESULT_FILE_NAME}" ]; then RESULT_FILE_NAME="result.txt"; fi

./tpcc_start -w15 -r10 -l${TIME} -i60 | tee temp-result.txt

cat temp-result.txt | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee ${RESULT_FILE_NAME}
cat ${RESULT_FILE_NAME}