#!/bin/bash
./clean.sh &> /dev/null
#DATA_POINTS=( 1 10 )
DATA_POINTS=( 1 10 100 1000 10000 100000 1000000 10000000 100000000 ) #1000000000 )
RESULTS_FILE="local_benchmark.dat"
NUM_TESTS=10
echo "#checkpoint_time,restore_time,total_time" > $RESULTS_FILE
echo "# Non-Diskless" >> $RESULTS_FILE
for i in "${DATA_POINTS[@]}";
do
    echo "Running non-diskless w/ $i points"
    echo "# $i points" >> $RESULTS_FILE
    for (( j=0; j<$NUM_TESTS; j++ ));
    do
        echo "\t- Test #$j"
        ./run.sh $i
        sudo ./migration.py --name eureka
        ./clean.sh
    done
done
echo "# Diskless" >> $RESULTS_FILE
for i in "${DATA_POINTS[@]}";
do
    echo "Running diskless w/ $i points"
    echo "# $i points" >> $RESULTS_FILE
    for (( j=0; j<$NUM_TESTS; j++ ));
    do
        echo "\t- Test #$j"
        ./run.sh $i
        sudo ./migration.py --name eureka --diskless
        ./clean.sh
    done
done
