#!/bin/bash
ip li delete br1
ip netns del namespace1
ip netns del namespace2
