#!/bin/bash
# Experiment 1
#sudo criu dump \
#    -t  `pidof counter` \
#    --images-dir images/3 \
#    --prev-images-dir ../2 \
#    --shell-job \
#    --track-mem

#Experiment 2
sudo criu dump \
    -t  `pidof counter` \
    --images-dir images/3 \
    --prev-images-dir ../2 \
    --shell-job \
    --track-mem \
    --auto-dedup

# Experiment 3
#sudo criu dump \
#    -t  `pidof counter` \
#    --images-dir images/3 \
#    --prev-images-dir ../2 \
#    --shell-job \
#    --track-mem \
#    --page-server \
#    --address 127.0.0.1 \
#    --port 9999
