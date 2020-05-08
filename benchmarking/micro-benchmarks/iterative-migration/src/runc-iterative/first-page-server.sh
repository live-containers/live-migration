#!/bin/bash
# Page Server
sudo criu page-server \
    --port 9999 \
    --images-dir ./ps-images/1/
