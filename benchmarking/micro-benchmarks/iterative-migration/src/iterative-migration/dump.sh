#!/bin/bash
# Last dump

sudo criu dump \
    -t  `pidof counter` \
    --images-dir images/3 \
    --prev-images-dir ../2 \
    --shell-job \
    --track-mem
