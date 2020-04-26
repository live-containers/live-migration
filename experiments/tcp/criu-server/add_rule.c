#include <errno.h> /* perror */
#include <stdio.h> /* fprintf */
#include <stdlib.h> /* exit */
#include <string.h> /* sprintf */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait */
#include <unistd.h> /* fork */

void usage()
{
    fprintf(stdout, "./add_rule <src_addr> <src_port> <dst_addr> <dst_port>\n");
    exit(0);
}

/* Run 'sudo iptables -S' to see the current chains */
int main(int argc, char *argv[])
{
    int rc = 0;
    if (argc != 5)
        usage();

    /* Parse arguments */
    char *src_addr = argv[1];
    char *src_port = argv[2];
    char *dst_addr = argv[3];
    char *dst_port = argv[4];

    /* Prepare command */
    char *inp_args[] = {"iptables", "-t", "filter", "-I", "INPUT", "--protocol",
        "tcp", "-m", "mark", "!", "--mark", "0xc114", "--source", dst_addr,
        "--sport", dst_port, "--destination", src_addr, "--dport", src_port,
        "-j", "DROP", NULL};
    char *out_args[] = {"iptables", "-t", "filter", "-I", "OUTPUT", "--protocol",
        "tcp", "-m", "mark", "!", "--mark", "0xc114", "--source", src_addr,
        "--sport", src_port, "--destination", dst_addr, "--dport", dst_port,
        "-j", "DROP", NULL};

    /* Make INPUT rule */
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("error running fork");
        exit(0);
    }
    if (pid == 0)
    {
        rc = execv("/usr/sbin/iptables", inp_args);
        if (rc == -1)
        {
            perror("error running execv");
            exit(0);
        }
    }
    if (pid > 0)
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        fprintf(stdout, "child exitted w/ status: %d\n", WEXITSTATUS(wstatus));
    }

    /* Make OUTPUT rule */
    pid = fork();
    if (pid < 0)
    {
        perror("error running fork");
        exit(0);
    }
    if (pid == 0)
    {
        rc = execv("/usr/sbin/iptables", out_args);
        if (rc == -1)
        {
            perror("error running execv");
            exit(0);
        }
    }
    if (pid > 0)
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        fprintf(stdout, "child exitted w/ status: %d\n", WEXITSTATUS(wstatus));
    }
     
     
    return 0;
}
