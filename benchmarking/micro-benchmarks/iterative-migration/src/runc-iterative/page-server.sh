#!/bin/bash
sudo criu page-server --port 9999 --images-dir ./images/$1/
#sudo criu page-server --auto-dedup --port 9999 --images-dir ./images/$1/
