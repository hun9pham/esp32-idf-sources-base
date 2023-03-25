#include <stdio.h>
#include <stdint.h>

#include "app.h"
#include "app_data.h"
#include "app_log.h"
#include "task_list.h"

#include "fa.h"
#include "modules_inc.h"
#include "sys_inc.h"

#define TAG "APP"

#define ESP_LOG_MSG_DIS

static void staNetConnectedCb(void) {
	mqttClientSett_t mqttClientInfo = {
		.portNum = adtMQTT_BROKER_PORT,
		.uriBroker = adtMQTT_BROKER_URI,
		.tId = -1,
		.username = adtMQTT_USERNAME,
		.password = adtMQTT_PASSWORD
	};
	mqttConnInit(&mqttClientInfo);
}

static void softInit() {
	watchdogInit(10000);
	staNetInit(adtSTA_SSID, adtSTA_PASS, staNetConnectedCb);
}

static void hardInit() {
	ledLifeInit();
}

void appStartInit() {
#if defined(ESP_LOG_MSG_DIS)
	esp_log_level_set("*", ESP_LOG_ERROR);
#endif

	softInit();
	hardInit();
	freeAssistantStart(TaskList, OS_END_TASK_ID);
	APP_LOG(TAG, "Rst: %s", getRstReason());

	while (1) {
		vTaskDelay(pdMS_TO_TICKS(portMAX_DELAY));
	}
}
