#include "shell.h"
#include "stdio.h"
#include "proc.h"

int main(int argc, char **argv)
{
    print("ZHIOS v1.0.0\n\n");
    int *ptr = (int *)(0x3ff000 - sizeof(int));
    printf("Argc: %d, stack: %d\n", argc, *ptr);
    while (1)
    {
        char buffer[1024];
        print("> ");
        read_line(buffer, 1024, true);
        print("\n");
        start_process(buffer);
        print("\n");
    }
}
