#include "stdio.h"
#include <stdarg.h>
#include <stdbool.h>
#include "stdlib.h"

extern void system_putchar(char c);
extern char system_getchar();

void putchar(char c)
{
    system_putchar(c);
}

char getchar()
{
    char val = 0;
    do
    {
        val = system_getchar();
    } while (val == 0);
    return val;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    const char *p;
    int ival;
    char *sval;
    char cval;

    va_start(ap, fmt);
    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }
        switch (*++p)
        {
        case 'd':
            ival = va_arg(ap, int);
            print(itoa(ival));
            break;
        case 's':
            sval = va_arg(ap, char *);
            print(sval);
            break;
        case 'c':
            cval = (char)va_arg(ap, int);
            putchar(cval);
            break;
        default:
            putchar(*p);
            break;
        }
    }
    va_end(ap);
    return 0;
}

void read_line(char *buffer, int max, bool echo_key)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        char key = getchar();
        if (key == 13)
        {
            break;
        }
        if (echo_key)
        {
            putchar(key);
        }
        if (key == 0x08 && i >= 1)
        {
            buffer[i - 1] = 0x00;
            i -= 2;
            continue;
        }
        buffer[i] = key;
    }
    buffer[i] = 0x00;
}
