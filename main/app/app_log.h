#ifndef __APP_LOG_H
#define __APP_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>

#define APP_DBG_EN
#define APP_LOG_EN
#define APP_DBG_SIG_EN

#define APP_PRINT					    printf

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#if defined(APP_DBG_EN)
#define APP_DBG(fmt, ...)       	    APP_PRINT(KBLU"[DBG] " fmt "\r\n" KNRM, ##__VA_ARGS__)
#else
#define APP_DBG(fmt, ...)
#endif

#if defined(APP_LOG_EN)
#define APP_LOG(tag, fmt, ...)          APP_PRINT(KGRN "[" tag "] " KYEL fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define APP_LOG(tag, fmt, ...)
#endif

#if defined(APP_DBG_SIG_EN)
#define APP_DBG_SIG(tag, fmt, ...)      APP_PRINT(KMAG "[SIG] " KGRN tag " -> " KYEL fmt "\r\n" KNRM, ##__VA_ARGS__)
#else
#define APP_DBG_SIG(tag, fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif
