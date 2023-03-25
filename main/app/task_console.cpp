#include <stdio.h>

#include "app.h"
#include "app_log.h"
#include "app_data.h"
#include "task_list.h"
#include "task_console.h"

#include "fa.h"
#include "modules_inc.h"
#include "sys_inc.h"

#define TAG "TaskConsole"

static int8_t csHelp(uint8_t *argv);
static int8_t csRst(uint8_t *argv);
static int8_t csCloud(uint8_t *argv);
static int8_t csSystem(uint8_t *argv);

static cmdLineStruct_t lgnCmdTable[] = {
	{ (const int8_t*)"help"		, csHelp, 			(const int8_t*)"Help commands"	 	},
	{ (const int8_t*)"rst"	    , csRst, 		    (const int8_t*)"Restart system"		},
    { (const int8_t*)"cloud"    , csCloud, 			(const int8_t*)"Cloud interface"	},
	{ (const int8_t*)"sys"	    , csSystem, 		(const int8_t*)"System interface"	},

	{ (const int8_t*)0			, (pfCmdFunc)0, 	(const int8_t*) 0 					}
};

void TaskConsoleCallback(void *params) {
	syncAllTaskStarted();
	APP_LOG(TAG, "Started");

	(void)params;
	uint8_t ch;

	for (;;) {
		ch = getchar();

		if (ch == '\r' || ch == '\n') {
			APP_PRINT("\r\n");

			switch (cmdLineParser(lgnCmdTable, ' ')) {
			case CMD_SUCCESS:
			break;

			case CMD_NOT_FOUND:
			break;

			case CMD_TOO_LONG:
			break;

			case CMD_TBL_NOT_FOUND:
			break;

			default:
			break;
			}

			APP_PRINT("- ");
		}
		else if (ch == 8 || ch == 127) { /* Backspace & Delelte */
			cmdLineClrChar();
		}
		else if ((ch == ' ') || (ch >= 65 && ch <= 126)){
			cmdLinePutChar(ch);
		}

		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

int8_t csHelp(uint8_t *argv) {
	(void)argv;

	APP_PRINT("\r\nHelp commands:\r\n");
	for (uint8_t id = 0; id < sizeof(lgnCmdTable) / sizeof(lgnCmdTable[0]) - 1; ++id) {
		APP_PRINT("  -%s: %s\r\n", lgnCmdTable[id].cmd, lgnCmdTable[id].Str);
	}

	return 0;
}

int8_t csRst(uint8_t *argv) {
	(void)argv;
    APP_PRINT("\'rst\': RestartSystem\r\n");
	freeAssistantPostEvent(OS_TASK_SYS_ID, SYSTEM_SOFT_REBOOT, NULL, 0);

	return 0;
}

int8_t csCloud(uint8_t *argv) {
    (void)argv;

	switch (getCmdLineParserCounter()) {
	case 1: {
		if (*(argv + 6) == 'h') {
			APP_PRINT("Usage:\r\n");
			APP_PRINT("\tcloud [option]\r\n");
			APP_PRINT("Option\r\n");
			APP_PRINT("\th : Help\r\n");
			APP_PRINT("\ts : Subcribe\r\n");
			APP_PRINT("\tp : Publish\r\n");
			APP_PRINT("\tpub {topic} {msg} : MQTT Publish API\r\n");
			APP_PRINT("\tsub {topic} : MQTT Subcribe API\r\n");
			APP_PRINT("\tunsub {topic} : MQTT Unsubcribe API\r\n");
		}
		else if (*(argv + 6) == 's') {
			if (mqttSubcribe(adtMQTT_SUB_TOPIC, QoS_MSG_LEVEL0) == MQTT_CONN_OK) {
				APP_LOG(TAG, "Subcribe: %s", adtMQTT_SUB_TOPIC);
			}
		}
		else if (*(argv + 6) == 'p') {
			char *pubMsgStr = (char*)"Omnidirectional";

			if (mqttPublish(adtMQTT_PUB_TOPIC, pubMsgStr, strlen(pubMsgStr), QoS_MSG_LEVEL0, false) == MQTT_CONN_OK) {
				APP_LOG(TAG, "Publish: %s, len: %d", pubMsgStr, strlen(pubMsgStr));
			}
		}
		else {

		}
	}
	break;


	case 2: {
        if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"sub") == 0) {
			char *topicStr = (char*)cmdLineGetAttr(2);
			if (mqttSubcribe(topicStr, QoS_MSG_LEVEL0) == MQTT_CONN_OK) {
				APP_LOG(TAG, "Subcribe: %s", topicStr);
			}
		}
		else if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"unsub") == 0) {
			char *topicStr = (char*)cmdLineGetAttr(2);
			if (mqttUnsubcribe(topicStr, QoS_MSG_LEVEL0) == MQTT_CONN_OK) {
				APP_LOG(TAG, "Unsubcribe: %s", topicStr);
			}
		}
		else if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"dsn") == 0) {
			char *domain = (char*)cmdLineGetAttr(2);
			char addrIpStr[20];
	
			dsnChangeUriToHostIp((const char*)domain, addrIpStr);
			APP_LOG(TAG, "Hostname: %s", addrIpStr);
			APP_LOG(TAG, "Hostname: %p", &addrIpStr[0]);
		}
		else {

		}
    }
    break;

	case 3: {
        if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"pub") == 0) {
			char *topicStr = (char*)cmdLineGetAttr(2);
			char *msgStr = (char*)cmdLineGetAttr(3);

			if (mqttPublish(topicStr, msgStr, strlen(msgStr), QoS_MSG_LEVEL0, false) == MQTT_CONN_OK) {
				APP_LOG(TAG, "Publish: %s, len: %d", topicStr, strlen(msgStr));
			}
		}
    }
    break;

    default: {
		APP_PRINT("Type \'cloud h\' for help\r\n");
	}
    break;
    };

	return 0;
}

int8_t csSystem(uint8_t *argv) {
    (void)argv;

	uint8_t *cmd = cmdLineGetAttr(1);

	if (strcmp((const char*)cmd, "ip") == 0) {
		APP_LOG(TAG, "Sta WiFi Ip: %s", staNetGetIp() != NULL ? staNetGetIp() : "");
		APP_LOG(TAG, "Ap WiFi Ip:  %s", apNetGetIp() != NULL ? apNetGetIp() : "");
	}
	else if (strcmp((const char*)cmd, "heap") == 0) {
		APP_LOG(TAG, "Free memory: %ld bytes", esp_get_free_heap_size());
	}
	else {
		APP_PRINT("Usage:\r\n");
		APP_PRINT("\tsys [option]\r\n");
        APP_PRINT("Option\r\n");
		APP_PRINT("\tip: IP Address\r\n");
		APP_PRINT("\theap: Get free heap size\r\n");
	}
 
	return 0;
}