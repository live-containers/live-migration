#!/bin/bash
docker run \
    --name test \
    --security-opt seccomp:unconfined \
    alpine:3.9
