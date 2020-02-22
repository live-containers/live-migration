#!/bin/bash
OUT_FILE=redis_$1.dat
for (( i=0; i<$1; i++ ))
do
    echo "SET key$i hello$i" >> $OUT_FILE
done
