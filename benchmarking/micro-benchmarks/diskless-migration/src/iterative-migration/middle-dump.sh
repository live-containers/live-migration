#!/bin/bash
# Second pre-dump.

#COUNTER_PID=`pidof counter`
REDIS_PID=$1

sudo criu pre-dump \
    -t ${REDIS_PID} \
    --shell-job \
    --images-dir images/2 \
    --prev-images-dir ../1 \
    --track-mem
