#include "string.h"
#include <stdint.h>
#include <stddef.h>

char* itoa(int value, char* str, int base) {
    if (base < 2 || base > 16) {
        str[0] = '\0';
        return str;
    }

    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;

    // handle negative numbers for base 10 only
    if (value < 0 && base == 10) {
        value = -value;
        *ptr++ = '-';
        ptr1++;
    }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789ABCDEF"[tmp_value - value * base];
    } while (value);

    *ptr-- = '\0';

    // reverse the string (excluding sign)
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }

    return str;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, int n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++; s2++; n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
int atoi(const char *str) {
    int res = 0;
    int sign = 1;
    int i = 0;

    // Handle optional sign
    if (str[0] == '-') {
        sign = -1;
        i++;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9')
            break; // stop at non-digit
        res = res * 10 + (str[i] - '0');
    }

    return sign * res;
}

char *strchr(const char *str, int c) {
    while (*str) {
        if (*str == (char)c)
            return (char *)str;
        str++;
    }
    return NULL;
}


int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}
