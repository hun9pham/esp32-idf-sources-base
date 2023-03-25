#include <stdio.h>

#include "app.h"
#include "app_log.h"
#include "task_list.h"
#include "task_system.h"

#include "fa.h"
#include "modules_inc.h"
#include "sys_inc.h"

#define TAG "TaskSystem"

static void systemStartInit() {
	freeAssistantSetTimer(OS_TASK_SYS_ID, SYSTEM_PING_ALIVE, SYSTEM_ALIVE_NOFITY_INTERVAL, true);
}

static void systemEventHandler(EventStruct_t *evSystem) {
	switch (evSystem->evSig) {
	case SYSTEM_PING_ALIVE: {
		ledLifeBlink();
		watchdogRst();
	}
	break;

	case SYSTEM_SOFT_REBOOT: {
		APP_DBG_SIG(TAG, "SYSTEM_SOFT_REBOOT");

		softReset();
	}
	break;

	default:
	break;
	}
}

void TaskSystemCallback(void *params) {
	syncAllTaskStarted();
	APP_LOG(TAG, "Started");

	systemStartInit();

	EventStruct_t evSystem = {
		.tId = 0,
		.evSig = 0,
		.evPayload = NULL,
		.evLen = 0
	};

	for (;;) {
		freeAssistantGetEvent(OS_TASK_SYS_ID, &evSystem);
		systemEventHandler(&evSystem);
		freeAssistantClearEvent(&evSystem);
	}
}
