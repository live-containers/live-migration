## runc-iterative

+ Experiment:
    1. Run container
    2. Run the binaries: `pre-dump.sh` `middle-dump.sh`, and `dump.sh` sequentially.
    3. Report the size of `pages.img` file.

1. Runc w/ pre-dump and auto-dedup (disk | tmpfs):
    + Dir 1: 11 M | 
    + Dir 2: 6.1 M
    + Dir 3: 6.1 M
    + Ratio: 55%

2. Runc w/ pre-dump w/out auto-dedup:
    + Dir 1: 8.4 M
    + Dir 2: 4.1 M
    + Dir 3: 4.1 M
    + Ratio: 48%

3. Runc w/ pre-dump w/ page-server w/out auto-dedup (disk | tmpfs):
    + Dir 1: 8.4M  
    + Dir 2: 4.1M
    + Dir 3: 4.1M
    + Ratio: 48%

4. Runc w/ pre-dump w/ page-server w/ auto-dedup and tmpfs:
    + Dir 1: 8.4M
    + Dir 2: 4.1M
    + Dir 3: 4.1M
    + Ratio: 48%

+ Open questions:
    + Why does `auto-dedup` flag add more to the base image (and less compression)?
        - It seems it only works in the "diskless" setting where images are stored in RAM (makes sense).
          See here: https://criu.org/Memory_images_deduplication.
        - Even in the diskless setting, via runc it does nothing.

==============================================================================

## iterative-migration w/ criu

+ All experiments w/ tmpfs
+ The experiment literally does nothing (it sleeps), I wonder what does 12K are, see Observations.

1. Criu w/ pre-dump w/out auto-dedup:
    + Dir 1: 92K
    + Dir 2: 12K
    + Dir 3: 12K
    + Ratio: 13%

2. Criu w/ pre-dump w/ auto-dedup:
    + Dir 1: 92K
    + Dir 2: 12K
    + Dir 3: 12K
    + Ratio: 13%

+ Observations:
    + Every time you do a pre-dump or a dump, the process is frozen and if it was in the middle of, for instance, a sleep, it will resume outside of the sleep.
