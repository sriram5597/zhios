#include "shell.h"
#include "stdio.h"
#include "proc.h"

int main(int argc, char **argv)
{
    print("ZHIOS v1.0.0\n\n");
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
