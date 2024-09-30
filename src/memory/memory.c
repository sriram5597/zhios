#include "memory.h"

void* memset(void* ptr, int n, size_t size) {
    char* ptr_ch = (char *) ptr;
    for (int i =0; i < size; i++) {
        ptr_ch[i] = (char) n;
    }
    return ptr;
}
