#include "string.h"
#include "terminal/terminal.h"

int strlen(const char *str)
{
    int len = 0;
    while (str[len])
    {
        len++;
    }
    return len;
}

bool is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

int atoi(char ch)
{
    return (int)(ch - '0');
}

char itoa(int num)
{
    return num + '0';
}

void strcpy(const char *src, char *dest)
{
    while (*src != 0)
    {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = 0x00;
}

void strncpy(const char *src, char *dest, int n)
{
    int i = 0;
    for (; i < n; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

char to_lower_char(char ch)
{
    if (ch >= 'A' && ch <= 'Z')
    {
        ch = (ch - 'A') + 'a';
    }
    return ch;
}

int strncmp(const char *s1, const char *s2, int n)
{
    unsigned char c1, c2;
    while (n-- > 0)
    {
        c1 = (unsigned char)*s1;
        c2 = (unsigned char)*s2;
        if (c1 != c2)
        {
            return c1 - c2;
        }
        if (c1 == '\0' || c2 == '\0')
        {
            return 0;
        }
        s1++;
        s2++;
    }
    return 0;
}

int istrncmp(const char *s1, const char *s2, int n)
{
    unsigned char c1, c2;
    while (n-- > 0)
    {
        c1 = to_lower_char((unsigned char)*s1);
        c2 = to_lower_char((unsigned char)*s2);
        if (c1 != c2)
        {
            return c1 - c2;
        }
        if (c1 == '\0' || c2 == '\0')
        {
            return 0;
        }
        s1++;
        s2++;
    }
    return 0;
}

void rstrip(const char *s1, char *s2)
{
    unsigned char c1 = (unsigned char)*s1;
    while (c1 != '\0' && c1 != 0x20)
    {
        *s2 = c1;
        s1++;
        c1 = *s1;
        s2++;
    }
    *s2 = '\0';
}

void strcat(const char *s1, const char *s2, char *dest)
{
    unsigned char *c1 = (unsigned char *)s1;
    unsigned char *c2 = (unsigned char *)s2;
    while (*c1 != '\0')
    {
        *dest = *c1;
        c1++;
        dest++;
    }
    while (*c2 != '\0')
    {
        *dest = *c2;
        c2++;
        dest++;
    }
    *dest = '\0';
}