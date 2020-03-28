#include "time_utils.h"

// We assume this is small and does not overflow
double timeval_to_milis(struct timeval *t)
{
    return t->tv_sec * 1000 + t->tv_usec / 1000;
}
