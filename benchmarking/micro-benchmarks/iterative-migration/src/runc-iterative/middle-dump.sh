#!/bin/bash
# Pre-dump number 2. Need to specify the parent image path.

# Without page server
sudo runc checkpoint \
    --pre-dump \
    --parent-path ../1/ \
    --image-path ./images/2/ \
    eureka

# With page server
#sudo runc checkpoint \
#    --pre-dump \
#    --page-server 127.0.0.1:9999 \
#    --parent-path ../1/ \
#    --image-path ./images/2/ \
#    eureka
