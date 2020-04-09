#!/bin/bash
sudo criu dump \
    -t  `pidof counter` \
    --leave-running \
    --track-mem \
    --images-dir images/2 \
    --shell-job \
    --prev-images-dir ../1
