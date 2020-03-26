#!/bin/bash
sudo runc delete -f $(sudo runc list -q)
sudo runc list
