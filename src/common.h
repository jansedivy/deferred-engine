#pragma once

#include <stdio.h>
#include <stdarg.h>

inline void formatString(char* buf, int buf_size, const char* fmt, va_list args) {
    int val = vsnprintf(buf, buf_size, fmt, args);
    if(val == -1 || val >= buf_size){
        buf[buf_size-1] = '\0';
    }
}

#define arrayCount(list) (sizeof(list) / sizeof(list[0]))

