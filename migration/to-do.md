## List of things to do divided in big  topics: (Updated: March 27 2020)

**Two paths to follow:**

1. Polish current code and do a thorough evaluation and benchmarking.
  + Dettach experiments from migration code.
  + Come up with different experiments (even in the Redis setting)
  + Measure what is transfered over the wire (memory)
  + Installation script (w/ key generation?)
  + Eventually integrate w/ runc

2. Implement the orchestrator for distributed checkpointing.

==============================================================================

Breakdown of different topics:

**Orchestrator:**
+ Design
  - Features:
    i. Act as DNS
    ii. Act as image server
    iii. Act as key manager (?)
  - Preconditions:
    i. Currently all keys must be distributed among all participants (need root access)
      * Maybe we could create a user (like Docker does)

**Experiments:**
+ Run experiments externally (detach completely from migration).
+ Add timing options for the migration script.
+ Play with the link (throuput) between VMs

**Software Engineering:**
+ Better/more structured logging.
+ Define structure for experiment files.

