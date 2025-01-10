#ifndef ISR80H_H
#define ISR80H_H

enum SystemCommands
{
    SYTEM_COMMAND0_SUM,
    SYTEM_COMMAND1_PRINT
};

void register_system_commands();

#endif