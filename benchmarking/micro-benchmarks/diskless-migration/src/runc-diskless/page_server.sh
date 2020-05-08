#!/bin/bash
# Page Server
sudo criu page-server \
    --port 9999 \
    --images-dir ./dst-images/
