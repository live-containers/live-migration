#!/bin/bash
SERVER_PPID=$(ps j `pidof -s server` | awk 'NR>1 {print $1}')
echo $SERVER_PPID
sudo criu dump \
    -t $SERVER_PPID \
    --images-dir images/1 \
    --shell-job \
    --tcp-established
