#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "argparser.h"

int main(int argc, char **argv)
{
    char str[] = "Hello from c";
    printf("Argc: %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("Arg %d: %s \n", i, argv[i]);
    }
    struct CommandArgument *root = arg_parse(str, strlen(str));
    struct CommandArgument *current = root;
    while (current)
    {
        printf("%s -> ", current->argument);
        current = current->next;
    }
    printf("\n");
out:
    while (1)
    {
    }
    return 0;
}