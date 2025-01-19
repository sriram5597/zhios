#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char **argv)
{
    char str[] = "Hello from c";
    char *tokens[100];
    printf("Tokenizing string...\n");

    int token_count = str_token(str, tokens, ' ');

    printf("------------\n");
    for (int i = 0; i < token_count; i++)
    {
        printf("Token %d: %s\n", i + 1, tokens[i]);
    }
    while (1)
    {
    }
    return 0;
}