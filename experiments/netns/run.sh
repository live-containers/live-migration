#!/bin/bash
# This script sets up two different network namespaces connected through a bridge in the host network.
# All three namespaces are reachable from within each other.
# Further, it demonstrates how to checkpoint and restore _external_ namespaces and re-attach back to them on restore.
# Even though we use nameless namespaces, we bind volumes so they become somewhat named.
# 
# References:
# - https://linux-blog.anracom.com/2017/10/30/fun-with-veth-devices-in-unnamed-linux-network-namespaces-i/
# - https://linux-blog.anracom.com/2017/11/12/fun-with-veth-devices-linux-bridges-and-vlans-in-unnamed-linux-network-namespaces-ii/
# - http://man7.org/linux/man-pages/man8/ip-netns.8.html

set -e

CRIU=/home/carlos/criu/criu/criu
LOG_LEVEL=3
NS_PATH="./ns"
IMAGE_PATH="./images"
NS_1=${NS_PATH}/netns-1
NS_2=${NS_PATH}/netns-2
HOST_BRIDGE=br1
BASE_IP=192.168.1.0
HOST_IP=192.168.1.10
IP_1=192.168.1.11
IP_2=192.168.1.12
VETH_1=veth1
VPEER_1=br-veth1
VETH_2=veth2
VPEER_2=br-veth2

# Clean environment
clean () {
    echo "DEBUG: Clean Working Environment"
    SERVER="_run.sh"
    SLEEP="_sleep.sh"
    sudo rm -f output1 output2 pidfile1 pidfile2 
    sudo rm -f *.log
    sudo rm -f ip1
    sudo rm -fr images
    sudo umount $NS_1 || /bin/true
    sudo umount $NS_2 || /bin/true
    sudo rm -f ./ns/* || /bin/true
    sudo kill -9 $(ps -ef | grep ${SERVER} | awk '{ print $2 }') || /bin/true
    sudo kill -9 $(ps -ef | grep ${SLEEP} | awk '{ print $2 }') || /bin/true
    sudo kill -9 $(ps -ef | grep ${IP_1} | awk '{ print $2 }') || /bin/true
    sudo kill -9 $(ps -ef | grep ${IP_2} | awk '{ print $2 }') || /bin/true
    sudo ip li delete ${HOST_BRIDGE} || /bin/true
    echo "DEBUG: Done cleaning."
}

# Clean Environment
clean
sleep 3

if [[ $1 == "clean" ]]; then
    echo "DEBUG: Entered in only clean mode. Exitting."
    exit 0
fi

# Create Namespaces Directory and Files
echo "DEBUG: Create Namespaces Directory and Files"
mkdir -p ${NS_PATH}
mkdir -p ${IMAGE_PATH} 
touch ${NS_1}
touch ${NS_2}

# Run the client daemon
echo "DEBUG: Run the client daemon"
setsid unshare --net=${NS_1} bash _run.sh pidfile1 tmp1.log 1 < /dev/zero &> output1 &
setsid unshare --net=${NS_2} bash _sleep.sh pidfile2 tmp2.log 2 < /dev/zero &> output2 &

# Get PIDs
echo "DEBUG: Get Client PIDs"
sleep 1
while :; do
	test -f pidfile1 && test -f pidfile2 && break;
	sleep 0.1
done
HOST_PID=$$
PID_1=$(< pidfile1)
PID_2=$(< pidfile2)

# Get iNodes and FD for the different namespaces, needed for C/R.
echo "DEBUG: Get iNodes and FD for the different namespaces, needed for C/R"
INO_1=$(ls -iL ${NS_1} | awk '{ print $1 }')
INO_2=$(ls -iL ${NS_2} | awk '{ print $1 }')
exec 33< ${NS_1}
exec 34< ${NS_2}

# Set up the bridge in the host network namespace
echo "DEBUG: Set up the bridge in the host network namespace"
ip link add name ${HOST_BRIDGE} type bridge
ip addr add ${HOST_IP}/24 brd + dev ${HOST_BRIDGE}
ip link set ${HOST_BRIDGE} up
ip link add ${VETH_1} netns ${PID_1} type veth peer name ${VPEER_1} netns ${HOST_PID} # fails: File exists
ip link add ${VETH_2} netns ${PID_2} type veth peer name ${VPEER_2} netns ${HOST_PID}
ip link set ${VPEER_1} master ${HOST_BRIDGE}
ip link set ${VPEER_2} master ${HOST_BRIDGE}
ip link set ${VPEER_1} up
ip link set ${VPEER_2} up

# Configure IPs and bring devices up
echo "DEBUG: Configure IP and bring Devices Up"
nsenter -t ${PID_1} --net=${NS_1} ip link set dev lo up
nsenter -t ${PID_2} --net=${NS_2} ip link set dev lo up
nsenter -t ${PID_1} --net=${NS_1} ip addr add ${IP_1}/24 dev ${VETH_1}
nsenter -t ${PID_1} --net=${NS_1} ip link set ${VETH_1} up
nsenter -t ${PID_2} --net=${NS_2} ip addr add ${IP_2}/24 dev ${VETH_2}
nsenter -t ${PID_2} --net=${NS_2} ip link set ${VETH_2} up

# Add Gateway route in each namespace
echo "DEBUG: Add Gatway Route in Each Namespace"
nsenter -t ${PID_1} --net=${NS_1} ip route add default via ${HOST_IP}
nsenter -t ${PID_2} --net=${NS_2} ip route add default via ${HOST_IP}
iptables -t nat -A POSTROUTING -s ${BASE_IP}/24 -j MASQUERADE

# Make IP Available for Client
echo ${IP_1} > ip1

read -n 1 -s -r -p "CONTROL: Press any key to Checkpoint\n"

# Checkpoint
${CRIU} dump \
    -v${LOG_LEVEL} \
    -t ${PID_1} \
    -o dump.log \
    -D images \
    --tcp-established \
    --external net[${INO_1}]:${NS_1} --external net[${INO_2}]:${NS_2}
cat images/dump.log | grep -B 5 Error || echo "DEBUG: Dump ok"

# TODO: Migrate from Net-NS 1 to Net-NS 2
nsenter -t ${PID_1} --net=${NS_1} ip addr del ${IP_1}/24 dev ${VETH_1}
nsenter -t ${PID_2} --net=${NS_2} ip addr add ${IP_1}/24 dev ${VETH_2}

read -n 1 -s -r -p "CONTROL: Press any key to Restore\n"

# Restore
${CRIU} restore \
    -v${LOG_LEVEL} \
    -o restore.log \
    -D images \
    --tcp-established \
    --inherit-fd fd[33]:${NS_2} \
    --inherit-fd fd[34]:${NS_1} -d
    #--inherit-fd fd[33]:${NS_1} \
    #--inherit-fd fd[34]:${NS_2} -d
cat images/restore.log | grep -B 5 Error || echo "DEBUG: Restore ok"

CR_INO_1=$(ls -iL /proc/${PID_1}/ns/net | awk '{ print $1 }')
CR_INO_2=$(ls -iL /proc/${PID_2}/ns/net | awk '{ print $1 }')
[ "${INO_1}" -ne "${CR_INO_1}" ] && {
	echo FAIL
	exit 1
}
[ "${INO_2}" -ne "${CR_INO_2}" ] && {
	echo FAIL
	exit 1
}
echo "DEBUG: C/R OK"
exit 0
