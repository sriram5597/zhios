#ifndef STRING_H
#define STRING_H

#include<stdbool.h>

int strlen(const char* ptr);
bool is_digit(char ch);
int atoi(char ch);
char itoa(int num);
void strcpy(const char* src, char* dest);

#endif