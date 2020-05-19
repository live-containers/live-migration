#!/bin/bash
# Declare Variables
CLIENT_IP=192.168.56.103
IPERF3=/home/carlos/iperf/src/iperf3
LOG_DIR=./iperf3-log
IMAGES_DIR=./images

# Set up Environment
mkdir -p ${LOG_DIR}

# Run iPerf3 server
echo "Bootstrapping Server..."
setsid ${IPERF3} \
    -s --port 9999 \
    --json \
    --interval 0.1 \
    --logfile ${LOG_DIR}/server.json \
    --one-off &> /dev/null < /dev/null &
SERVER_PID=$!

sleep 3

# Run iPerf3 client in remote machine
echo "Bootstrapping Client..."
ssh carlos@${CLIENT_IP} "/home/carlos/tcp-established/iperf3_client.sh"

sleep 10

# CRIU Dump
echo "Dumping server..."
sudo criu pre-dump \
    -t ${SERVER_PID} \
    --images-dir ${IMAGES_DIR} \
    --tcp-established 

# Get client data
sleep 10
echo "Done!"
