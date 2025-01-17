#ifndef ISR80H_H
#define ISR80H_H

enum SystemCommands
{
    SYTEM_COMMAND0_SUM,
    SYTEM_COMMAND1_PRINT,
    SYSTEM_COMMAND2_PUTCHAR,
    SYSTEM_COMMAND3_GETKEY,
    SYSTEM_COMMAND4_MALLOC,
    SYSTEM_COMMAND5_FREE,
};

void register_system_commands();

#endif