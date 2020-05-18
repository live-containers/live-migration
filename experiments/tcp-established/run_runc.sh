#!/bin/bash
# Declare Variables
CLIENT_IP=192.168.56.103
IPERF3=/home/carlos/iperf/src/iperf3
LOG_DIR=./iperf3-log
IMAGES_DIR=/home/carlos/criu-lm/experiments/tcp-established/images
CWD=$(pwd)

# Set up Environment
mkdir -p ${LOG_DIR}

# Run iPerf3 server
cd /home/carlos/runc-containers/iperf3-server
sudo runc run eureka &> /dev/null < /dev/null &
cd ${CWD}

sleep 3

# Run iPerf3 client in remote machine
echo "Bootstrapping Client..."
ssh carlos@${CLIENT_IP} "/home/carlos/tcp-established/iperf3_client.sh"

sleep 10

# CRIU Dump
echo "Dumping server..."
sudo runc checkpoint \
    --image-path ${IMAGES_DIR} \
    --tcp-established \
    eureka

sleep 2

# CRIU Restore
echo "Restoring server..."
cd /home/carlos/runc-containers/iperf3-server 
sudo runc restore \
    --image-path ${IMAGES_DIR} \
    --tcp-established \
    eureka-restored
cd ${CWD}

# Get client data
sleep 1
echo "Getting client results..."
#scp carlos@192.168.56.103:tcp-established/iperf3-log/client.json ./iperf3-log/

# Process data
#python3 _process_data.py
#cat ./client.dat
