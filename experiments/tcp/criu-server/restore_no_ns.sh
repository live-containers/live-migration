#!/bin/bash
sudo criu restore -v4 \
    --images-dir images/1 \
    --work-dir logs \
    --shell-job \
    --tcp-established
