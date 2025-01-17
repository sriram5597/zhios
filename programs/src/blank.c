#include "console/console.h"
#include "stdlib/stdlib.h"

int main(int argc, char **argv)
{
    print("Hello world from c...\n");
    void *ptr = malloc(512);
    if (ptr != 0)
    {
        print("Allocated memory...\n");
    }
    free(ptr);

    while (1)
    {
        char c = getkey();
        if (c != 0)
        {
            print(c);
        }
    }
    return 0;
}