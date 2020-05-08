#!/bin/bash
# Last dump

#COUNTER_PID=`pidof counter`
REDIS_PID=$1

sudo criu dump \
    -t  ${REDIS_PID} \
    --images-dir images/3 \
    --prev-images-dir ../2 \
    --shell-job \
    --track-mem
