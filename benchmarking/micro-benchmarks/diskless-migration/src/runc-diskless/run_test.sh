#!/bin/bash
HOME=$(pwd)
NUM_TESTS=1000
acc=0
acc2=0
for (( i=1; i<=$NUM_TESTS; i++ ))
do
    cd /home/carlos/runc-containers/counter/ && sudo ./run.sh && cd $HOME
    sudo ./clean.sh
    sudo ./page_server.sh &
    ts=$(date +%s%N)
    sudo ./dump.sh
    scp -r ./src-images/* ./dst-images/
    time_elapsed=$((($(date +%s%N) - $ts)/1000000))
    acc=$(($acc + $time_elapsed))
    acc2=$(($acc2 + $time_elapsed * $time_elapsed))
    echo "Test $i: $time_elapsed"
done
avg=$(bc <<<"scale=2; $acc / $NUM_TESTS")
std=$(bc <<<"scale=2; sqrt($acc2 / $NUM_TESTS - $avg * $avg)")
echo "Average: $avg"
echo "Std: $std"
