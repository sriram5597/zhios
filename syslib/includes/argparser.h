#ifndef ARGPARSER_H
#define ARGPARSER_H

struct CommandArgument
{
    char argument[512];
    struct CommandArgument *next;
};

struct CommandArgument *arg_parse(const char *command, int len);

#endif