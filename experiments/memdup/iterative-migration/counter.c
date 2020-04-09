#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile int keep_running = 1;

void int_handler(int tmp)
{
    keep_running = 0;
}

int main()
{
    int count = 0;
    signal(SIGINT, int_handler);

    fprintf(stdout, "Current count: %i\n", count++);
    while (keep_running)
    {
        sleep(20);
    }

    return 0;
}
