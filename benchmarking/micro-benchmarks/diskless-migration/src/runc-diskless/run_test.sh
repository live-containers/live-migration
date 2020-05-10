#!/bin/bash
HOME=$(pwd)
IP=192.168.56.103
NUM_TESTS=100
acc=0
acc2=0
for (( i=1; i<=$NUM_TESTS; i++ ))
do
    cd /home/carlos/runc-containers/counter/ && sudo ./run.sh && cd $HOME
    #cd /home/carlos/runc-containers/redis/ && sudo ./run_redis.sh 10000000 && cd $HOME
    sudo ./clean.sh
    #sudo ./page_server.sh &
    #ssh carlos@${IP} "/home/carlos/runc-diskless/page_server.sh &> /dev/null < /dev/null &"
    ssh carlos@${IP} "/home/carlos/runc-diskless/clean.sh"
    ts=$(date +%s%N)
    sudo ./dump.sh
    #scp -r ./src-images/* ./dst-images/
    scp -r ./src-images/* carlos@${IP}:runc-diskless/dst-images/
    time_elapsed=$((($(date +%s%N) - $ts)/1000000))
    acc=$(($acc + $time_elapsed))
    acc2=$(($acc2 + $time_elapsed * $time_elapsed))
    echo "Test $i: $time_elapsed"
done
avg=$(bc <<<"scale=2; $acc / $NUM_TESTS")
std=$(bc <<<"scale=2; sqrt($acc2 / $NUM_TESTS - $avg * $avg)")
echo "Average: $avg"
echo "Std: $std"
