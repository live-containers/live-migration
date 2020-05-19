#!/bin/bash
IPERF3=/home/carlos/iperf/src/iperf3
LOG_DIR=./iperf3-log
SERVER_IP=192.168.56.101

mkdir -p ${LOG_DIR}

${IPERF3} \
    -c ${SERVER_IP} \
    --port 9999 \
    --json \
    --logfile ${LOG_DIR}/client.json \
    --time 60
