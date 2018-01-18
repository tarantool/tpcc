#!/usr/bin/env bash

if [ ! -n "${TIME}" ]; then TIME=600; fi
if [ ! -n "${WARMUP_TIME}" ]; then WARMUP_TIME=10; fi
if [ ! -n "${RESULT_FILE_NAME}" ]; then RESULT_FILE_NAME="result.txt"; fi

/opt/tpcc/tpcc_start -w15 -r10 -l${TIME} -i${TIME} | tee temp-result.txt

cat temp-result.txt | grep -e '<TpmC>' | grep -oP '\K[0-9.]*' | tee ${RESULT_FILE_NAME}
cat ${RESULT_FILE_NAME}