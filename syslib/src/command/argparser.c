#include "argparser.h"
#include "string.h"
#include "stdlib.h"

struct CommandArgument *arg_parse(const char *command, int len)
{
    char temp[1024];
    strcpy(command, temp);
    char *tokens[16];
    int tokens_count = str_token(temp, tokens, ' ');
    struct CommandArgument *current = (struct CommandArgument *)malloc(sizeof(struct CommandArgument) * tokens_count);
    struct CommandArgument *root = current;
    struct CommandArgument *prev = 0;
    for (int i = 0; i < tokens_count; i++)
    {
        strcpy(tokens[i], current->argument);
        if (prev != 0)
        {
            prev->next = current;
        }
        prev = current;
        current++;
    }
    return root;
}
