#!/bin/bash
echo "Container Started"
sudo ./clean.sh
echo "Clean Done"
sudo ./pre-dump.sh
echo "Pre-Dump Done"
redis-benchmark -h $(< /home/carlos/runc-containers/redis/.ip) -p 6379 -n 10000 &> /dev/null
sudo ./middle-dump.sh
echo "Middle-Dump Done"
redis-benchmark -h $(< /home/carlos/runc-containers/redis/.ip) -p 6379 -n 10000 &> /dev/null
sudo ./dump.sh
echo "Dump Done"
D1=$(ls -lah ./images/1/pages-1.img | awk '{ print $5; }')
D2=$(ls -lah ./images/2/pages-1.img | awk '{ print $5; }')
D3=$(ls -lah ./images/3/pages-1.img | awk '{ print $5; }')
echo "Test finished: -D1: $D1 -D2: $D2 -D3: $D3"
