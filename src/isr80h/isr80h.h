#ifndef ISR80H_H
#define ISR80H_H

enum SystemCommands
{
    SYTEM_COMMAND0_SUM,
    SYTEM_COMMAND1_PRINT,
    SYSTEM_COMMAND2_PUTCHAR,
    SYSTEM_COMMAND3_GETKEY
};

void register_system_commands();

#endif