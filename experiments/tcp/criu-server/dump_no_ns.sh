#!/bin/bash
# Dump server in the host namespace.
# --ext-unix-sk is required when there's no connection established.
# Surprisingly enough, dumping the server with an established connection
# and restoring it works alright without this parameter, but without an
# established connection does not.
#
# This script is to be used together with ./restore_no_ns.sh
SERVER_PPID=$(ps j `pidof -s server` | awk 'NR>1 {print $1}')
echo $SERVER_PPID
sudo criu dump -v4 \
    -t $SERVER_PPID \
    --images-dir images/1 \
    --work-dir logs \
    --shell-job \
    --ext-unix-sk \
    --tcp-established
