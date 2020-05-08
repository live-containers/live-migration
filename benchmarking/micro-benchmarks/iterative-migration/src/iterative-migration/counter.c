#include <signal.h>
#include <stdio.h>
#include <stdlib.h> /* atoi */
#include <unistd.h>

static volatile int keep_running = 1;

void int_handler(int tmp)
{
    keep_running = 0;
}

int main(int argc, char *argv[])
{
    int count = 0;
    int inc = 0;
    if (argc > 1)
        inc = atoi(argv[1]);
    signal(SIGINT, int_handler);

    fprintf(stdout, "Current count: %i\n", count++);
    if (inc)
    {
        while (keep_running)
        {
            fprintf(stdout, "Current count: %i\n", count++);
            sleep(2);
        }
    }
    else 
    {
        while (keep_running)
            sleep(20);
    }

    return 0;
}
