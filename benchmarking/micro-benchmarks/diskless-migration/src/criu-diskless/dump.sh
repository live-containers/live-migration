#!/bin/bash
# Diskless dump (with page-server)
PID=$1
ADDRESS=192.168.56.103
ADDRESS=127.0.0.1

sudo criu dump \
    -t $PID \
    --shell-job \
    --ext-unix-sk \
    --images-dir ./src-images/
    #--page-server \
    #--address $ADDRESS \
    #--port 9999 \
