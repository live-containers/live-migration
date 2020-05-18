#!/bin/bash
sudo criu restore \
    --images-dir ./images \
    --tcp-established
