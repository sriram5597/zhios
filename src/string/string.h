#ifndef STRING_H
#define STRING_H

#include <stdbool.h>

int strlen(const char *ptr);
bool is_digit(char ch);
int atoi(char ch);
char itoa(int num);
void strcpy(const char *src, char *dest);
int strncmp(const char *s1, const char *s2, int n);
void rstrip(const char *s1, char *s2);
void strcat(const char *s1, const char *s2, char *dest);
int istrncmp(const char *s1, const char *s2, int n);

#endif