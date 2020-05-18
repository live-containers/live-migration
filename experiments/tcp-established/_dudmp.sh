sudo criu dump \
    -t `pidof iperf3` \
    --images-dir ./images \
    --tcp-established
