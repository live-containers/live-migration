#!/bin/bash
BINARY="_run.sh"
sudo rm output pidfile1 pidfile2 pidfile
sudo rm *.log
sudo rm -r images
sudo umount ./ns/netns-1
sudo umount ./ns/netns-2
sudo rm ./ns/*
sudo kill -9 $(ps -ef | grep $BINARY | awk '{ print $2 }')
