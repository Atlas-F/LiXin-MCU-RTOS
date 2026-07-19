// ============ debug_utils.h（公共调试头文件）============
#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

// #include <stdio.h>   // 如果嵌入式环境，换成你的串口输出头文件
#include "usart.h"

#ifdef DEBUG_MODE
    #define DEBUG_PRINT(fmt, ...) \
        printf("[%s:%d] " fmt "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define DEBUG_ASSERT(cond) \
        do { if (!(cond)) { \
            printf("ASSERT FAIL: %s, file %s, line %d\r\n", #cond, __FILE__, __LINE__); \
            while(1); \
        }} while(0)
#else
    #define DEBUG_PRINT(fmt, ...)   ((void)0)   // 啥也不干，编译器会优化掉
    #define DEBUG_ASSERT(cond)      ((void)0)
#endif

#endif // DEBUG_UTILS_H
