#!/bin/bash
docker run -d \
    --name js-looper \
    -p 8080:80 \
    --security-opt seccomp:unconfined \
    alpine-counter:latest
