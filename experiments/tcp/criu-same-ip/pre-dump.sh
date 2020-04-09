#!/bin/bash
sudo criu dump \
    -t  `pidof counter` \
    --leave-running \
    --track-mem \
    --images-dir images/1 \
    --shell-job
