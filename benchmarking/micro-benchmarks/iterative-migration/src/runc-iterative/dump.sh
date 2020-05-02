#!/bin/bash
# Experiment 1
#sudo runc checkpoint \
#    --parent-path ../2/ \
#    --auto-dedup \
#    --image-path ./images/3/ \
#    eureka

# Experiment 2
#sudo runc checkpoint \
#    --parent-path ../2/ \
#    --image-path ./images/3/ \
#    eureka

# Experiment 3
#sudo runc checkpoint \
#    --page-server 127.0.0.1:9999 \
#    --parent-path ../2/ \
#    --image-path ./images/3/ \
#    eureka

# Experiment 4
#sudo runc checkpoint \
#    --page-server 127.0.0.1:9999 \
#    --auto-dedup \
#    --parent-path ../2/ \
#    --image-path ./images/3/ \
#    eureka

# Experiment 5
sudo runc checkpoint \
    --page-server 192.168.56.103:9999 \
    --parent-path ../2/ \
    --image-path ./images/3/ \
    eureka
