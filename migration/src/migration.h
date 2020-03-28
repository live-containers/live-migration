#ifndef MIGRATION_H
#define MIGRATION_H
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "net_utils.h"
#include "time_utils.h"

/* Container Experiments */
#define EXPERIMENT_REDIS            0

#if BENCHMARK
    /* For each iteration we store:
     *
     * 1. Prepare migration: create local and remote dirs and start page-server.
     * 2. Pre-dump: run pre-dump
     * 3. Transfer files
     * 4. Housekeeping: create symlink, update dirs
     */
    #define NUM_PROFILING_EVENTS            4
#endif

/* Container Config Paths */
#define RUNC_REDIS_PATH             "/home/carlos/runc-containers/redis"

/* Other Settings */
#define LOG_FILE                    "/var/log/runc-lm"
#define BENCHMARK_BASE_PATH         "../../benchmarking/iterative-migration"

struct migration_args;

#endif /* MIGRATION_H */
