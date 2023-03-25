#ifndef __FreeRTOS_ASSISTANT_H
#define __FreeRTOS_ASSISTANT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/message_buffer.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: FreeRTOS Assistant
 *  Note: Public definitions
 *----------------------------------------------------------------------------*/
// #define faRELEASE
#define faASSISTANT_NAME		"Dynamo"

#if defined(faRELEASE)
    #define faBUILD				"Release"
    #define faVERSION 			"1.1"
    #define faASSERT_VAL  		(0)
#else
    #define faBUILD				"Debug"
    #define faVERSION 			"1.0"
    #define faASSERT_VAL  		(1)
#endif

#define faRST	   				"\e[0m"
#define faGRN					"\e[0;32m"
#define faKNRM  				"\x1B[0m"
#define faKRED  				"\x1B[31m"
#define faKYEL  				"\x1B[33m"
#define faKMAG  				"\x1B[35m"
#define faKCYN  				"\e[0;36m"
#define faKWHT  				"\x1B[37m"

#define faMAILBOX_SIZE			(8)
#define faUSER_DEFINE_SIGNAL	(10)
#define faSTACK_SIZE_UNIT		(sizeof(uint32_t))

#define faENTRY_ATOMIC()		portDISABLE_INTERRUPTS()
#define faEXIT_ATOMIC()			portENABLE_INTERRUPTS()

#define faASSERT(assert)   		freeAssistantAssert(assert, __FILE__, __LINE__)

/* Enumeration ---------------------------------------------------------------*/
typedef enum eTaskRank {
	BRONZE = 1,
	SILVER,
	GOLD,
	PLANTINUM,
	DIAMOND,
} TaskRankType_t;

typedef enum eStackSize {
	STACK_SZ_MIN 	= faSTACK_SIZE_UNIT * 256,
	STACK_SZ_MEDIUM = faSTACK_SIZE_UNIT * 512,
	STACK_SZ_HIGH 	= faSTACK_SIZE_UNIT * 1024,
	STACK_SZ_HYPER 	= faSTACK_SIZE_UNIT * 2048,
	STACK_SZ_MAX 	= faSTACK_SIZE_UNIT * 4096,
} StackSizeType_t;

/* Typedef -------------------------------------------------------------------*/
typedef QueueHandle_t MailboxType_t;
typedef TimerHandle_t EventTimerType_t;

typedef struct t_EventStructure {
	uint8_t tId;
	uint8_t evSig;
	uint8_t *evPayload;
	uint32_t evLen;
} EventStruct_t;

typedef struct t_TaskStructure {
	uint8_t tId;
	void (*tCallback)(void *);
	StackSizeType_t tStackDepth; /* eStackSize * faSTACK_SIZE_UNIT */
	TaskRankType_t tPrio;
	const char *const tCaller;
	MailboxType_t tMailbox;
} TaskStruct_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void freeAssistantStart(TaskStruct_t *taskTbl, uint8_t taskTblSize);
extern void syncAllTaskStarted(void);

extern void freeAssistantPostEvent(uint8_t tId, uint8_t evSig, uint8_t *evPayload, uint32_t evLen);
extern void freeAssistantGetEvent(uint8_t tId, EventStruct_t *evt);
extern void freeAssistantClearEvent(EventStruct_t *evt);

extern EventTimerType_t freeAssistantSetTimer(uint8_t tId, uint8_t evSig, uint32_t period, bool isReload);
extern void freeAssistantRemoveTimer(EventTimerType_t mEvtTimId);

extern void freeAssistantAssert(uint8_t assert, const char *file, uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /* __FreeRTOS_ASSISTANT_H */
