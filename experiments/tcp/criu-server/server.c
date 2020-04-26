#define _GNU_SOURCE
#include <sched.h>

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SA struct sockaddr

static volatile int keep_running = 1;

static void int_handler(int tmp)
{
    keep_running = 0;
}

static void print_ip(unsigned int ip)
{
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;   
    printf("%d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]);        
}

static int serve_connection(int sockfd)
{
    char buf[1024];
    int rd, wr;

    while (1)
    {
        rd = read(sockfd, buf, sizeof buf);
        if (rd == 0)
            break;

        if (rd < 0)
        {
            fprintf(stderr, "can't read socket\n");
            return 1;
        }

        wr = 0;
        while (wr < rd)
        {
            int w;
            w = write(sockfd, buf + wr, rd - wr);
            if (w <= 0)
            {
                fprintf(stderr, "can't write to socket\n");
                return 1;
            }
            wr += w;
        }
    }

    fprintf(stdout, "done\n");

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: ./server <net_ns> <port>\n");
        return 1;
    }
    
    int port;
    char *netns_name = (char *) malloc(128);
    memset(netns_name, '\0', 128);
    FILE *net_fd;
    int fd;
    if (argc == 3)
    {
        /* Set Network Namespace */
        // WARN: use snprintf in the future
        sprintf(netns_name, "/var/run/netns/%s", argv[1]);
        net_fd = fopen(netns_name, "r");
        if (net_fd == NULL)
        {
            perror("fopen failed");
            return 1;
        }
        fd = fileno(net_fd);
        if (fd == -1)
        {
            perror("fileno failed");
            return 1;
        }
        if (setns(fd, CLONE_NEWNET) != 0)
        {
            perror("setns failed");
            return 1;
        }
        struct stat file_stat;
        if (fstat(fd, &file_stat) < 0)
        {
            perror("fstat failed");
            return 1;
        }
        fprintf(stdout, "Netns: %s has fd -> %i and inode -> %li\n",
                netns_name, fd, file_stat.st_ino);
        port = atoi(argv[2]);
        fclose(net_fd);
    }
    else
    {
        port = atoi(argv[1]);
    }


    int sockfd;
    unsigned int len;
    struct sockaddr_in servaddr, cli;

    signal(SIGCHLD, SIG_IGN);
    //signal(SIGINT, int_handler);
    struct sigaction sa;
    sa.sa_handler = &int_handler;
    sigaction(SIGINT, &sa, NULL);

    /*
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1)
    {
        fprintf(stderr, "error creating socket\n");
        goto exit;
    }
    fprintf(stdout, "created server socket\n");
    memset(&servaddr, '\0', sizeof servaddr);

    //Assign IP, Port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(sockfd, (struct sockaddr *) &servaddr, sizeof servaddr)) != 0)
    {
        fprintf(stderr, "binding server failed\n");
        goto exit;
    }
    fprintf(stdout, "binded server\n");

    if ((listen(sockfd, 5)) != 0)
    {
        fprintf(stderr, "listen failed\n");
        goto exit;
    }
    fprintf(stdout, "listening...\n");

    */
    while (keep_running)
    {
        sleep(10);
        /*
        int pid, connfd;
        memset(&cli, '\0', sizeof cli);
        len = sizeof(struct sockaddr_in);
        connfd = accept(sockfd, (struct sockaddr *) &cli, &len);
        if (connfd < 0)
        {
            fprintf(stderr, "accept failed\n");
            goto exit;
        }
        fprintf(stdout, "connection from: ");
        print_ip(cli.sin_addr.s_addr);
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "fork failed\n");
            close(connfd);
            goto exit;
        }    
        if (pid > 0) // parent
            close(connfd);
        else
        {
            close(sockfd);
            if (serve_connection(connfd) != 0)
            {
                fprintf(stderr, "serve connection failed\n");
                exit(1);
            }
            close(connfd);
            exit(0); 
        }
        */
    }
    goto exit;

exit:
    //close(sockfd);
    return 0;
}
