#!/bin/bash
# First pre-dump

sudo criu pre-dump \
    -t  `pidof counter` \
    --images-dir images/1 \
    --track-mem \
    --shell-job 
