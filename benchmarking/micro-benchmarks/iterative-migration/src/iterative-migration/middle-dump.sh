#!/bin/bash
# Second pre-dump.

sudo criu pre-dump \
    -t  `pidof counter` \
    --shell-job \
    --images-dir images/2 \
    --prev-images-dir ../1 \
    --track-mem
