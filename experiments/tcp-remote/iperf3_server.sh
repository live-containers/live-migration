#!/bin/bash
IPERF3=/home/carlos/iperf/src/iperf3
LOG_DIR=./iperf3-log
mkdir -p ${LOG_DIR}

setsid ${IPERF3} \
    -s --port 9999 \
    --json \
    --logfile ${LOG_DIR}/server.json \
    --one-off &> /dev/null < /dev/null &
