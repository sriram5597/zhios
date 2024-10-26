#include "string.h"

int strlen(const char* str) {
    int len = 0;
    while(str[len]) {
        len++;
    }
    return len;
}

bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

int atoi(char ch) {
    return (int)(ch - '0');
}

char itoa(int num) {
    return num + '0';
}

void strcpy(const char* src, char* dest) {
    while (src != 0) {
        *dest = *src;
        src++;
        dest++;
    }
    *dest =0x00;
}
