#!/bin/bash
sudo criu dump \
    -t  `pidof client` \
    --images-dir images/1 \
    --shell-job \
    --tcp-established
