#!/bin/bash
sudo criu restore \
    --images-dir images/1 \
    --shell-job \
    --tcp-established
