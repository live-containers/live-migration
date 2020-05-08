#!/bin/bash
# Dump. Note that we also specify the parent image path.

# Without page-server
sudo runc checkpoint \
    --parent-path ../2/ \
    --image-path ./images/3/ \
    eureka

# With page-server
#sudo runc checkpoint \
#    --page-server 127.0.0.1:9999 \
#    --parent-path ../2/ \
#    --image-path ./images/3/ \
#    eureka
