#!/bin/bash
HOME=$(pwd)
IP=192.168.56.103
NUM_TESTS=10
acc=0
acc2=0
PID=-1
for (( i=1; i<=$NUM_TESTS; i++ ))
do
    # Run Application
    #cd /home/carlos/runc-containers/counter/
    #cd /home/carlos/runc-containers/redis/ 
    #sudo ./counter 0 &> /dev/null < /dev/null &
    #cd $HOME
    redis-server --port 9999 &> /dev/null < /dev/null &
    #redis-server --port 9999 &
    PID=$!
    sleep 3
    cat "/home/carlos/runc-containers/redis/data/redis_10000000.dat" | redis-cli -p 9999 --pipe
    #cat "/home/carlos/runc-containers/redis/data/redis_10.dat" | redis-cli -p 9999 --pipe
    redis-cli -p 9999 config set stop-writes-on-bgsave-error no
    # Clean Environment
    sudo ./clean.sh
    #ssh carlos@${IP} "/home/carlos/runc-diskless/clean.sh"
    # Run the page server
    #sudo ./page_server.sh &
    #ssh carlos@${IP} "/home/carlos/runc-diskless/page_server.sh &> /dev/null < /dev/null &"
    kill -0 $PID
    ts=$(date +%s%N)
    sudo ./dump.sh $PID
    # Copy the images
    scp -r ./src-images/* ./dst-images/
    #scp -r ./src-images/* carlos@${IP}:runc-diskless/dst-images/
    time_elapsed=$((($(date +%s%N) - $ts)/1000000))
    acc=$(($acc + $time_elapsed))
    acc2=$(($acc2 + $time_elapsed * $time_elapsed))
    echo "Test $i: $time_elapsed"
done
avg=$(bc <<<"scale=2; $acc / $NUM_TESTS")
std=$(bc <<<"scale=2; sqrt($acc2 / $NUM_TESTS - $avg * $avg)")
echo "Average: $avg"
echo "Std: $std"
