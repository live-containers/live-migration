#!/bin/bash
# Experiment 1
#sudo runc checkpoint \
#    --pre-dump \
#    --auto-dedup \
#    --image-path ./images/1/ \
#    eureka

# Experiment 2
#sudo runc checkpoint \
#    --pre-dump \
#    --image-path ./images/1/ \
#    eureka

# Experiment 3
#sudo runc checkpoint \
#    --pre-dump \
#    --page-server 127.0.0.1:9999 \
#    --image-path ./images/1/ \
#    eureka

# Experiment 4
#sudo runc checkpoint \
#    --pre-dump \
#    --auto-dedup \
#    --page-server 127.0.0.1:9999 \
#    --image-path ./images/1/ \
#    eureka

# Experiment 5
sudo runc checkpoint \
    --pre-dump \
    --page-server 192.168.56.103:9999 \
    --image-path ./images/1/ \
    eureka
