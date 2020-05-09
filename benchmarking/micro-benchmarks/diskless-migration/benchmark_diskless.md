# General Details:

+ Machine:
```
Linux debian 4.19.0-6-amd64 #1 SMP Debian 4.19.67-2+deb10u2 (2019-11-11) x86_64 GNU/Linux
(on VirtualBox)
```

+ Criu:
```
Version: 3.13
GitID: v3.13-292-g1a205330
```

+ Runc Details:
```
runc version 1.0.0-rc8
commit: 425e105d5a03fabd737a126ad93d62a9eeede87f
spec: 1.0.1-dev
```

+ Notes:

+ Experiments:
  1. Counter & Redis, local and remote.

## Diskless Migration w/ runc

+ Experiment:
    1. Run container
    2. Start the page server
    3. Dump
    4. Copy the remaining files

+ Results: avg and stdev of timing 3&4 multiple times. (1000 tests)

1. Counter local:
  + Diskless: 305.08 ms (14.59)
  + Non-diskless:

2. Counter remote:
  + Diskless:
  + Non-diskless:

3. Redis local:
  + Diskless: 897.10 ms (161.26)
  + Non-diskless:

4. Redis remote:
  + Diskless:
  + Non-diskless:

==============================================================================

## Iterative Migration w/ criu 

1. Counter without increasing:
    + Dir 1: 92K
    + Dir 2: 12K
    + Dir 3: 12K

2. Counter w/ increasing:
    + Dir 1: 96K
    + Dir 2: 24K
    + Dir 3: 24K
    + Ratio: 13%

3. Redis without shuffling:
    + Dir 1: 986M
    + Dir 2: 4.1M
    + Dir 3: 4.1M

4. Redis with shuffling:
    + Dir 1: 986M
    + Dir 2: 665M,51M,36M,33M,49M
    + Dir 3: 27M,20M,24M,24M,26M

+ Observations:
    + Every time you do a pre-dump or a dump, the process is frozen and if it was in the middle of, for instance, a sleep, it will resume outside of the sleep.
