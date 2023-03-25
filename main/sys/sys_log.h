#ifndef __SYS_LOG_H
#define __SYS_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>

#define SYS_LOG_EN

#define SYS_PRINT					    printf

#define SYS_KNRM  "\x1B[0m"
#define SYS_KRED  "\x1B[31m"
#define SYS_KGRN  "\x1B[32m"
#define SYS_KYEL  "\x1B[33m"
#define SYS_KBLU  "\x1B[34m"
#define SYS_KMAG  "\x1B[35m"
#define SYS_KCYN  "\x1B[36m"
#define SYS_KWHT  "\x1B[37m"

#if defined(SYS_LOG_EN)
#define SYS_LOG(tag, fmt, ...)          SYS_PRINT(SYS_KBLU "[" tag "] " SYS_KRED fmt SYS_KNRM "\r\n", ##__VA_ARGS__)
#else
#define SYS_LOG(tag, fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif
