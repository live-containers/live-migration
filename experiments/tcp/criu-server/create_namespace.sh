#!/bin/bash

# Define Variables
IFACE="enp0s3"
NS="criu-net"
VETH="veth1"
VPEER="vpeer1"
VETH_ADDR="10.200.1.1"
VPEER_ADDR="10.200.1.2"

trap cleanup EXIT

cleanup()
{
    ip li delete ${VETH} 2>/dev/null
}

# Remove namespace if it already exists
ip netns del $NS &>/dev/null

# Create namespace
ip netns add $NS

# Create veth link
ip link add ${VETH} type veth peer name ${VPEER}

# Add peer to namespace
ip link set ${VPEER} netns ${NS}

# Setup IP address of ${VETH}
ip addr add ${VETH_ADDR}/24 dev ${VETH}
ip link set ${VETH} up

# Setup IP ${VPEER}
ip netns exec $NS ip addr add ${VPEER_ADDR}/24 dev ${VPEER}
ip netns exec $NS ip link set ${VPEER} up
ip netns exec $NS ip link set lo up
ip netns exec $NS ip route add default via ${VETH_ADDR}

# Enable IP-forarding
echo 1 > /proc/sys/net/ipv4/ip_forward

# Flush forward rules
iptables -P FORWARD DROP
iptables -F FORWARD

# Flush nat rules
iptables -t nat -F

iptables -t nat -A POSTROUTING -s ${VPEER_ADDR}/24 -o ${IFACE} -j MASQUERADE

iptables -A FORWARD -i ${IFACE} -o ${VETH} -j ACCEPT
iptables -A FORWARD -o ${IFACE} -i ${VETH} -j ACCEPT

# Get into network namespace
ip netns exec $NS /bin/bash --rcfile <(echo "PS1=\"${NS}> \"")
