#include "stdio/stdio.h"
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

    char buffer[51];
    read_line(buffer, 50, true);
    printf("\nInput: %s", buffer);
    return 0;
}