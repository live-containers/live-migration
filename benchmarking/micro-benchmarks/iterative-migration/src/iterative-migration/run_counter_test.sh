#!/bin/bash
echo "Counter without increment:"
sudo ./counter 0 &> /dev/null &
sudo ./clean.sh
echo "Clean Done"
sudo ./pre-dump.sh
sleep 1
echo "Pre-Dump Done"
sudo ./middle-dump.sh
sleep 1
echo "Middle-Dump Done"
sudo ./dump.sh
echo "Dump Done"
D1=$(ls -lah ./images/1/pages-1.img | awk '{ print $5; }')
D2=$(ls -lah ./images/2/pages-1.img | awk '{ print $5; }')
D3=$(ls -lah ./images/3/pages-1.img | awk '{ print $5; }')
echo "Test finished: -D1: $D1 -D2: $D2 -D3: $D3"

echo "Counter with increment:"
sudo ./counter 1 &> /dev/null &
sudo ./clean.sh
echo "Clean Done"
sudo ./pre-dump.sh
sleep 1
echo "Pre-Dump Done"
sudo ./middle-dump.sh
sleep 1
echo "Middle-Dump Done"
sudo ./dump.sh
echo "Dump Done"
D1=$(ls -lah ./images/1/pages-1.img | awk '{ print $5; }')
D2=$(ls -lah ./images/2/pages-1.img | awk '{ print $5; }')
D3=$(ls -lah ./images/3/pages-1.img | awk '{ print $5; }')
echo "Test finished: -D1: $D1 -D2: $D2 -D3: $D3"
