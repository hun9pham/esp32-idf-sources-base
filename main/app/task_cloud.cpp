#include <stdio.h>
#include <string.h>

#include "app.h"
#include "app_log.h"
#include "app_data.h"
#include "task_list.h"
#include "task_cloud.h"

#include "fa.h"
#include "modules_inc.h"
#include "sys_inc.h"

#define TAG "TaskCloud"

static void cloudStartInit() {
	
}

static void cloudEventHandler(EventStruct_t *evCloud) {
	switch (evCloud->evSig) {
	case CLOUD_ENTRY_CONNECT_SERVER: {
		APP_DBG_SIG(TAG, "CLOUD_ENTRY_CONNECT_SERVER");
        
		APP_LOG(TAG, "MQTT_CONN: %s", mqttConnStatusGet() == CONN_CONNECTED ? "SUCCESS" : "FAILED");
		
	}
	break;

	case CLOUD_PERIODIC_PING_SERVER_ALIVE: {
		APP_DBG_SIG(TAG, "CLOUD_PERIODIC_CHECK_CONNECTION");

	}
	break;

	default:
	break;
	}
}

void TaskCloudCallback(void *params) {
	syncAllTaskStarted();
	APP_LOG(TAG, "Started");

	cloudStartInit();

	EventStruct_t evCloud = {
		.tId = 0,
		.evSig = 0,
		.evPayload = NULL,
		.evLen = 0
	};

	for (;;) {
		freeAssistantGetEvent(OS_TASK_CLOUD_ID, &evCloud);
		cloudEventHandler(&evCloud);
		freeAssistantClearEvent(&evCloud);
	}
}