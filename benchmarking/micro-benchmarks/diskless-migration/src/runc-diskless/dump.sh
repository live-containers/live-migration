#!/bin/bash
# Diskless dump (with page-server)
sudo runc checkpoint \
    --image-path ./src-images/ \
    eureka
    #--page-server 192.168.56.103:9999 \
    #--page-server 127.0.0.1:9999 \
