#!/bin/bash
# Experiment 1
#sudo criu pre-dump \
#    -t  `pidof counter` \
#    --shell-job \
#    --images-dir images/2 \
#    --prev-images-dir ../1 \

# Experiment 2
sudo criu pre-dump \
    -t  `pidof counter` \
    --shell-job \
    --images-dir images/2 \
    --prev-images-dir ../1 \
    --auto-dedup

# Experiment 3
#sudo criu pre-dump \
#    -t  `pidof counter` \
#    --images-dir images/2 \
#    --shell-job \
#    --prev-images-dir ../1 \
#    --page-server \
#    --track-mem \
#    --address 127.0.0.1 \
#    --port 9999
