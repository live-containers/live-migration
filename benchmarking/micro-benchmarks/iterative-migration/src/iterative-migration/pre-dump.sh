#!/bin/bash
# Experiment 1
#sudo criu pre-dump \
#    -t  `pidof counter` \
#    --images-dir images/1 \
#    --shell-job \

# Experiment 2
sudo criu pre-dump \
    -t  `pidof counter` \
    --images-dir images/1 \
    --shell-job \
    --auto-dedup

# Experiment 3
#sudo criu pre-dump \
#    -t  `pidof counter` \
#    --images-dir images/1 \
#    --shell-job \
#    --page-server \
#    --address 127.0.0.1 \
#    --port 9999
