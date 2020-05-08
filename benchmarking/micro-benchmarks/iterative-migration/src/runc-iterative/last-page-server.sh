#!/bin/bash
# Page Server
sudo criu page-server \
    --port 9999 \
    --prev-images-dir ../2/ \
    --images-dir ./ps-images/3/
