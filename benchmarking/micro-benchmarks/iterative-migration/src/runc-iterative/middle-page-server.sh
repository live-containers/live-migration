#!/bin/bash
# Page Server
sudo criu page-server \
    --port 9999 \
    --prev-images-dir ../1/ \
    --images-dir ./ps-images/2/
