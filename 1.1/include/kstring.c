#include "vfs.h"

void kstrncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

int kstrcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* kstrrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == c) {
            last = s;
        }
        s++;
    }
    return (char*)last;
}

int ksnprintf(char* str, size_t size, const char* format, ...) {
    size_t i = 0;
    while (*format && i < size - 1) {
        str[i++] = *format++;
    }
    str[i] = '\0';
    return i;
}

void kstrncat(char* dest, const char* src, size_t n) {
    size_t dest_len = 0;
    while (dest[dest_len] != '\0') dest_len++;
    
    size_t i;
    for (i = 0; i < n - dest_len - 1 && src[i] != '\0'; i++) {
        dest[dest_len + i] = src[i];
    }
    dest[dest_len + i] = '\0';
}

size_t kstrlen(const char* s) {
    size_t len = 0;
    while (s[len]!= '\0') len++;
    return len;
}