#!/bin/bash
ip netns exec $1 /bin/bash --rcfile <(echo "PS1=\"${1}> \"")
