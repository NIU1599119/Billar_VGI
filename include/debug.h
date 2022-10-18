#pragma once


#ifdef DEBUG
#include <cassert>
#include <stdio.h>

#define ASSERT(x) assert(x)

//                      BOLD;BLUE
#define INFO_COLOR "\033[1;34m"
//                      BOLD;GREEN
#define DEBUG_COLOR "\033[1;32m"
//                      BOLD;RED
#define ERROR_COLOR "\033[1;31m"
//                      DEFAULT
#define END_COLOR "\033[0m"

// colors are defined using ascii escape sequences

#define LOG_INFO(msg, ...) do {\
        fprintf(stdout, INFO_COLOR "[%s]" END_COLOR " : %s : %d : " msg " \n", \
                "LOG", __FILE__, __LINE__, ##__VA_ARGS__);\
    } while (0)

#define LOG_DEBUG(msg, ...) do {\
        fprintf(stdout, DEBUG_COLOR "[%s]" END_COLOR " : %s : %d : " msg " \n", \
                "DEBUG", __FILE__, __LINE__, ##__VA_ARGS__);\
    } while (0)

#define LOG_ERROR(msg, ...) do {\
        fprintf(stdout, ERROR_COLOR "[%s]" END_COLOR " : %s : %d : " msg " \n", \
                "ERROR", __FILE__, __LINE__, ##__VA_ARGS__);\
    } while (0)

#else
#define ASSERT(x)  
#define LOG_INFO(msg, ...)  
#define LOG_DEBUG(msg, ...)  
#define LOG_ERROR(msg, ...)  
#endif