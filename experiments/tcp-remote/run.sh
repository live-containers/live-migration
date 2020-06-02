#!/bin/bash
# Declare Variables
CLIENT_IP=192.168.56.103
SERVER_IP=192.168.56.110
IFACE=enp0s3
IPERF3=/home/carlos/iperf/src/iperf3
LOG_DIR=./iperf3-log
IMAGES_DIR=./images

# Set up Environment
mkdir -p ${LOG_DIR}
sudo ip addr add ${SERVER_IP}/24 dev ${IFACE}
sudo iptables --flush OUTPUT
sudo iptables --flush INPUT

# Run nc server
echo "Bootstrapping Server..."
setsid nc -l -s ${CLIENT_IP} -p 9999 &> /dev/null < /dev/null &
NC_PID=$!

sleep 3

# Run iPerf3 client in remote machine
echo "Bootstrapping Client..."
ssh carlos@${CLIENT_IP} "/home/carlos/tcp-established/iperf3_client.sh"

sleep 10

# CRIU Dump
echo "Dumping server..."
sudo criu dump \
    -t ${SERVER_PID} \
    --images-dir ${IMAGES_DIR} \
    --tcp-established &

sleep 2

# CRIU Restore
#echo "Restoring server..."
#sudo criu restore \
#    --images-dir ${IMAGES_DIR} \
#    --tcp-established
#
## Get client data
#sleep 1
#echo "Getting client results..."
#scp carlos@192.168.56.103:tcp-established/iperf3-log/client.json ./iperf3-log/

# Process data
#python3 _process_data.py
#cat ./client.dat
