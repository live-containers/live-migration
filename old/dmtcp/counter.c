#include <stdio.h>
#include <unistd.h>

int main()
{
    unsigned int count = 0;

    while (1)
    {
        printf("%2d\n", count++);
        fflush(stdout);
        sleep(2);
    }

    return 0;
}
