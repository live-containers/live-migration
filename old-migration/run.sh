#!/bin/bash
REDIS_PATH="/home/carlos/criu-demos/runc/redis/"
# Check if number of data points supplied
if [ -z "$1" ]
then
    NUM_ENTRIES=1
else
    NUM_ENTRIES=$1
fi
cd $REDIS_PATH && ./run_redis.sh $NUM_ENTRIES && cd -
