#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "app.h"
#include "app_log.h"

#include "fa.h"

#include "sys_inc.h"

void app_main() {
    vTaskDelay(pdMS_TO_TICKS(100));
    appStartInit();
}