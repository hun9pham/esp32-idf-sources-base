#ifndef __TASK_LIST_H
#define __TASK_LIST_H

#include "fa.h"
#include "task_system.h"
#include "task_cloud.h"
#include "task_console.h"

enum eTaskListId {
	OS_TASK_SYS_ID,
	OS_TASK_CLOUD_ID,
	OS_TASK_CONSOLE_ID,

	/* EOT */
	OS_END_TASK_ID,
};

extern TaskStruct_t TaskList[];

#endif /* __TASK_LIST_H */
