#define _GNU_SOURCE
#include <sched.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    FILE *net_fd = fopen("/var/run/netns/namespace1", "r");
    int fd = fileno(net_fd);
    fprintf(stdout, "This is my fd: %i\n", fd);
    if (setns(fd, CLONE_NEWNET) != 0)
        perror("setns");
    system("ip addr"); 
    fclose(net_fd);
    return 0;
}
