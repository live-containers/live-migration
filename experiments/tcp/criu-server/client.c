#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sockfd, port, rc;
    struct sockaddr_in addr;
    if (argc < 3)
    {
        fprintf(stderr, "haven't supplied enough arguments\n");
    }
    
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        fprintf(stderr, "can't create socket\n");
        return -1;
    }

    port = atoi(argv[2]);
    fprintf(stdout, "connecting to %s:%d\n", argv[1], port);
    memset(&addr, '\0', sizeof addr);
    addr.sin_family = AF_INET;
    rc = inet_aton(argv[1], &addr.sin_addr);
    if (rc < 0)
    {
        fprintf(stdout, "can't convert address\n");
        return -1;
    }
    addr.sin_port = htons(port);

    rc = connect(sockfd, (struct sockaddr *) &addr, sizeof addr);
    if (rc < 0)
    {
        fprintf(stderr, "can't connect to server\n");
        return -1;
    }

    int rval, val = 1;
    while (1)
    {
        write(sockfd, &val, sizeof val);
        rval = -1;
        read(sockfd, &rval, sizeof rval);
        fprintf(stdout, "Count: %d -> %d\n", val++, rval);
        sleep(2);
    }

    close(sockfd);
    return 0;
}
