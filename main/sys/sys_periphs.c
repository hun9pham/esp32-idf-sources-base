#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <setjmp.h>
#include <unistd.h>

#include "sdkconfig.h"

#include "sys_log.h"
#include "sys_periphs.h"

#define TAG "SysPeriphs"

void ledLifeInit() {
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

uint8_t ledLifeBlink() {
    static uint8_t ledState = 0;    

    gpio_set_level(BLINK_GPIO, ledState);
    ledState = !ledState;

    return ledState;
}
