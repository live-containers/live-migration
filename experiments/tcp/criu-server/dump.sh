#!/bin/bash
#Dump the server running within a network namespace. In order for the dump
#to work we need to keep the namespace running (i.e. not dump it).
#This is due to the fact that our application _joins_ an existing namespace
#(via setns) rathre than creating a new one (via unshare).
#
#--external expects us to pass the <inode> for the namespace (which is the
#inode for the file in `/var/run/netns/<namespace_name>`. Our sample app
#spits it to stdout.
CRIU_PATH=/home/carlos/criu/criu/criu
SERVER_PPID=$(ps j `pidof -s server` | awk 'NR>1 {print $1}')
sudo $CRIU_PATH dump \
    -t $SERVER_PPID \
    --config ./criu.conf \
    --external net\[$1\]:netns1
