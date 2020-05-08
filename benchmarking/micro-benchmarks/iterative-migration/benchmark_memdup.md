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
- `auto-dedup` does not work for some reason.
- no difference between w/ page server and without
- non-diskless (but it does not matter in this case)
- track-mem is set by default in runc

+ Experiments:
  1. Counter: c-based counter that just increments (if inc) or does nothing at all.
  2. Redis: redis database with 1e7 keys, 256M, (w/ or without movement). For the shuffling we run:
  `redis-benchmark -h <IP> -n 10000

## Iterative Migration w/ runc

+ Experiment:
    1. Run container
    2. Run the binaries: `pre-dump.sh` `middle-dump.sh`, and `dump.sh` sequentially.
      - I.e. two pre-dumps (Dir 1, and Dir 2) and one dump (Dir 3).
    3. Report the size of `pages.img` file.

1. Counter without increasing:
    + Dir 1: 92K 
    + Dir 2: 12K
    + Dir 3: 12K

2. Counter w/ increasing:
    + Dir 1: 92K
    + Dir 2: 24K
    + Dir 3: 24K

3. Redis without shuffling:
    + Dir 1: 906M  
    + Dir 2: 2.1M
    + Dir 3: 2.1M

4. Redis with shuffling:
    + Dir 1: 908M
    + Dir 2: 585M,45M,581M,29M,586M,590M,33M
    + Dir 3: 21M,22M,19M,20M,17M,21M,21M

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
