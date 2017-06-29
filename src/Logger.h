#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef LOG_CTRL_H
#define LOG_CTRL_H
#define LOG_CTRL_CONTINUE

#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4
#define NONE 7

#endif

#ifndef AUTHER_x000032001

extern int xxxLogLevel __attribute__((unused));

#endif

#ifdef LOG_CTRL_CONTINUE

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define SET_LOG_LEVEL(x) (xxxLogLevel = (x))

#define slogf(inputLV, format, ...)                                                                                                               \
    do {                                                                                                                                          \
        if ((inputLV) >= xxxLogLevel) {                                                                                                           \
            if (inputLV == DEBUG)                                                                                                                 \
                fprintf(stderr, CYN);                                                                                                             \
            if (inputLV == INFO)                                                                                                                  \
                fprintf(stderr, WHT);                                                                                                             \
            if (inputLV == WARN)                                                                                                                  \
                fprintf(stderr, YEL);                                                                                                             \
            if (inputLV == ERROR)                                                                                                                 \
                fprintf(stderr, RED);                                                                                                             \
            fprintf(stderr, "[%5d][%.9s] %.9s:%3d %.12s() # " format RESET, getpid(), #inputLV, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__); \
            fflush(stderr); \
        }                                                                                                                                         \
        if ((inputLV) >= ERROR) {                                                                                                                 \
            exit(1);                                                                                                                              \
        }                                                                                                                                         \
    } while (0)

#endif
