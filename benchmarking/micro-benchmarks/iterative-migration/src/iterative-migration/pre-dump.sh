#!/bin/bash
# First pre-dump

#COUNTER_PID=`pidof counter`
REDIS_PID=$1

sudo criu pre-dump \
    -t  ${REDIS_PID} \
    --images-dir images/1 \
    --track-mem \
    --shell-job 
