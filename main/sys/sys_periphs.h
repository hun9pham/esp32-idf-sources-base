#ifndef __SYS_PERIPHS_H
#define __SYS_PERIPHS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "driver/gpio.h"

#define BLINK_GPIO  2

extern void ledLifeInit();
extern uint8_t ledLifeBlink();

#ifdef __cplusplus
}
#endif

#endif