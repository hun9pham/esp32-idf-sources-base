#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "fa.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: OS_TASK_SYSTEM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define SYSTEM_ALIVE_NOFITY_INTERVAL        ( 1000 )

/* Define signal */
enum {
	SYSTEM_PING_ALIVE = faUSER_DEFINE_SIGNAL,
	SYSTEM_SOFT_REBOOT,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: OS_TASK_CONSOLE_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */


/*----------------------------------------------------------------------------*
 *  DECLARE: OS_TASK_CLOUD_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
enum {
	CLOUD_ENTRY_CONNECT_SERVER = faUSER_DEFINE_SIGNAL,
	CLOUD_PERIODIC_PING_SERVER_ALIVE,
};

/* Define signal */

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note:
 *----------------------------------------------------------------------------*/
extern void appStartInit();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H */
