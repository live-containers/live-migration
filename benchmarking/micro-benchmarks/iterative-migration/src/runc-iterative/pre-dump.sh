#!/bin/bash
# Pre-dump number 1

# Without page-server
sudo runc checkpoint \
    --pre-dump \
    --image-path ./images/1/ \
    eureka

# With page-server
#sudo runc checkpoint \
#    --pre-dump \
#    --page-server 127.0.0.1:9999 \
#    --image-path ./images/1/ \
#    eureka
