#!/bin/bash
CRIU_PATH=/home/carlos/criu/criu/criu
sudo $CRIU_PATH restore -d \
    --config ./criu.conf \
    --external net\[netns1\]:/run/netns/namespace1

    #--external veth\[veth1\]:br-veth1@br1 \
    #--tcp-established
    #--inherit-fd fd\[$1\]:netns1
