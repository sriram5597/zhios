#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char **argv)
{
    char str[] = "Hello from c";
    char **tokens = (char **)malloc(50);
    if (tokens == 0)
    {
        printf("Failed to allocate memory...");
        goto out;
    }
    printf("Tokenizing string...\n");

    int token_count = str_token(str, tokens, ' ');

    printf("------------\n");
    for (int i = 0; i < token_count; i++)
    {
        printf("Token %d: %s\n", i + 1, tokens[i]);
    }
out:
    free((void *)tokens);
    while (1)
    {
    }
    return 0;
}