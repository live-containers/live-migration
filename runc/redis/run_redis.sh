#!/bin/bash
sudo runc run -d eureka &> /dev/null < /dev/null
# Populate DB
cat "./data/redis_$1.dat" | redis-cli -h $(< .ip) --pipe
sudo runc list
