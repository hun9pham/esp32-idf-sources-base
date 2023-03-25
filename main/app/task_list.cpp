#include "task_list.h"

TaskStruct_t TaskList[] = {
	{	OS_TASK_SYS_ID		, TaskSystemCallback	, STACK_SZ_MEDIUM	, DIAMOND	, "TaskSystem",	NULL  },
	{	OS_TASK_CLOUD_ID	, TaskCloudCallback		, STACK_SZ_MEDIUM	, PLANTINUM	, "TaskCloud",	NULL  },
	{	OS_TASK_CONSOLE_ID	, TaskConsoleCallback	, STACK_SZ_HYPER	, SILVER	, "TaskConsole",NULL  },
};
