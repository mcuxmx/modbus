#ifndef __OSAL_LOG_H_
#define __OSAL_LOG_H_
#include <stdio.h>

#define LOG_DEBUG(x,...)    printf( __VA_ARGS__ )
#define LOG_INFO(x,...)     printf( __VA_ARGS__ )
#define LOG_ERROR(x,...)    printf( __VA_ARGS__ )

#define LOG_DEBUG_ENABLED(x)    0

#endif
