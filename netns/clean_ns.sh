#!/bin/bash
sudo ip li delete br1
sudo ip netns del namespace1
sudo ip netns del namespace2
