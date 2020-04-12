#define _GNU_SOURCE
#include <sched.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#define ASSERTMSG(condition, msg) _assert(condition, msg, __LINE__)

int _assert(int condition, const char msg_onerror[], int line)
{
    if (condition == 0)
    {
        fprintf(stderr, "[%s] on line %d\n", msg_onerror, line);
        fprintf(stderr, "errno=%d [%s]\n", errno, strerror(errno));
        exit(1);
    }
    return 0;
}

void unshare_ns()
{
    const int UNSHARE_FLAGS = CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS;
    const char *hostname = "cartainer";

    ASSERTMSG(-1 != unshare(UNSHARE_FLAGS), "unshare has failed");
    ASSERTMSG(-1 != sethostname(hostname, strlen(hostname)),
              "sethostname has failed");
}

void cartainer(char *exec_path, char *argv[])
{
    /* Must invoke unshare before forking. */
    unshare_ns();

    pid_t child_pid = fork();
    ASSERTMSG(-1 != child_pid, "fork PID 1 has failed");

    int status;

    if (child_pid == 0)
    {
        ASSERTMSG(-1 != (child_pid = fork()), "fork PID 2 has failed");

        /* We don't want argv to be PID 1, rather PID 2 */
        if (child_pid == 0)
            ASSERTMSG(-1 != execvp(exec_path, argv), "exec has failed");

        while (wait(&status) != -1 || errno != ECHILD);

        fprintf(stderr, "init died\n");
    }

    wait(&status);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
        return 0;

    cartainer(argv[1], &argv[1]);

    return 0;
}
