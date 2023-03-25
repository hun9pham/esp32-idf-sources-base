#include "fa.h"
#include "sys_inc.h"

#define TAG "freeAssistant"

/*----------------------------------------------------------------------------*
 *  DECLARE: FreeRTOS Assistant
 *  Note: Private definitions
 *----------------------------------------------------------------------------*/
#define faBANNER_PRINT_EN
#define faLOG_PRINT_EN

#define faPRINT         		printf

#if defined(faBANNER_PRINT_EN)
#define faBANNER(fmt, ...) 		faPRINT(faKCYN fmt faRST, ##__VA_ARGS__)
#else
#define faBANNER(fmt, ...)
#endif

#if defined(faLOG_PRINT_EN)
#define faLOG(tag, fmt, ...) 	faPRINT(faKRED "[" tag "] " faKMAG fmt faRST "\r\n", ##__VA_ARGS__)
#else
#define faLOG(tag, fmt, ...)
#endif

/* Typedef -------------------------------------------------------------------*/
typedef struct t_FreeRTOSAssistant {
    TaskStruct_t *taskManager;
    uint16_t nbrOfTasks;
} freeAssistant_t;

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static freeAssistant_t virtualAssistant = {
    .taskManager =  NULL,
    .nbrOfTasks = 0
};

static SemaphoreHandle_t syncTaskStarted = NULL;

/* Private function prototypes -----------------------------------------------*/
static void FreeRTOSAssistantIntro(void);
static void internalSoftTimerCb(TimerHandle_t timId);

/* Function implementation ---------------------------------------------------*/
/** -----------------------------------------------------
  * @brief  Initializes all tasks                       -
  * @param  taskTbl: Pointer points to task table list  -
  * @param  taskTblSize : Size of task table list       -
  * @retval None                                        -
  * ----------------------------------------------------*/
void freeAssistantStart(TaskStruct_t *taskTbl, uint8_t taskTblSize) {
	FreeRTOSAssistantIntro();

    virtualAssistant.taskManager = taskTbl;
    virtualAssistant.nbrOfTasks = taskTblSize;
    faLOG(TAG, "TaskTableLen: %d", virtualAssistant.nbrOfTasks);
    faASSERT(virtualAssistant.taskManager != NULL && virtualAssistant.nbrOfTasks != 0);

    syncTaskStarted = xSemaphoreCreateCounting(virtualAssistant.nbrOfTasks, virtualAssistant.nbrOfTasks);
    faASSERT(syncTaskStarted != NULL);

    for (uint8_t tId = 0; tId < virtualAssistant.nbrOfTasks; ++tId) {
        virtualAssistant.taskManager[tId].tMailbox = xQueueCreate(faMAILBOX_SIZE, sizeof(EventStruct_t));
        faASSERT(virtualAssistant.taskManager[tId].tMailbox != NULL);

        faLOG(TAG, "[TaskCreate] %s, Id: %d, StackSize: %d, Prio: %d, Cb: %p\r\n",
                virtualAssistant.taskManager[tId].tCaller,
                virtualAssistant.taskManager[tId].tId,
                virtualAssistant.taskManager[tId].tStackDepth * faSTACK_SIZE_UNIT,
                virtualAssistant.taskManager[tId].tPrio,
                virtualAssistant.taskManager[tId].tCallback
                );
        faASSERT(
            pdFAIL != xTaskCreate(virtualAssistant.taskManager[tId].tCallback,
                                    virtualAssistant.taskManager[tId].tCaller,
                                    virtualAssistant.taskManager[tId].tStackDepth,
                                    NULL,
                                    virtualAssistant.taskManager[tId].tPrio,
                                    NULL
                                )
                );
    }
}

/** -----------------------------------------------------------
  * @brief  Call this function to wait all tasks initialize   -
  * @param  void                                              -
  * @retval None                                              -
  * ----------------------------------------------------------*/
void syncAllTaskStarted() {
    faASSERT(pdFAIL != xSemaphoreTake(syncTaskStarted, portMAX_DELAY));

    while (uxSemaphoreGetCount(syncTaskStarted) > 0) {
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/** -------------------------------------------------------------------
  * @brief  Post event to task's mailbox                              -
  * @param  tId: Task identity                                        -
  * @param  evSig : Signal of event structure                         -
  * @param  evPayload : Data has carried by event signal              -
  * @param  evLen : Number of bytes data has carried by event signal  -
  * @retval None                                                      -
  * ------------------------------------------------------------------*/
void freeAssistantPostEvent(uint8_t tId, uint8_t evSig, uint8_t *evPayload, uint32_t evLen) {
    uint8_t *buf = NULL;

    faASSERT(tId < virtualAssistant.nbrOfTasks);

    if (evLen != 0 && evPayload != NULL) {
        buf = (uint8_t*)pvPortMalloc(evLen);
        faASSERT(buf != NULL);

        memcpy(buf, evPayload, evLen);
    }

    EventStruct_t evStructure = {
        .tId = tId,
        .evSig = evSig,
        .evPayload = buf,
        .evLen = evLen
    };

    faASSERT(uxQueueSpacesAvailable(virtualAssistant.taskManager[tId].tMailbox) > 0);
    xQueueSend(virtualAssistant.taskManager[tId].tMailbox, (void*)&evStructure, portMAX_DELAY);
}

/** -------------------------------------------------------------------
  * @brief  Get event from task's mailbox                             -
  * @param  tId: Task identity                                        -
  * @param  evt : Event structure needs to get data                   -
  * @retval None                                                      -
  * ------------------------------------------------------------------*/
void freeAssistantGetEvent(uint8_t tId, EventStruct_t *evt) {
    faASSERT(tId < virtualAssistant.nbrOfTasks);

    if (xQueueReceive(virtualAssistant.taskManager[tId].tMailbox, evt, portMAX_DELAY) == pdPASS) {
        /* TODO
         -
         -
        */
    }
}

/** --------------------------------------------------------------------
  * @brief  Clear event, free heap memory if it has dynamic allocated  -
  * @param  evt : Event structure needs to be clear                    -
  * @retval None                                                       -
  * -------------------------------------------------------------------*/
void freeAssistantClearEvent(EventStruct_t *evt) {
    if (evt->evLen != 0) {
        vPortFree(evt->evPayload);
    }
    memset(evt, 0, sizeof(EventStruct_t));
}

/** --------------------------------------------------------------------
  * @brief  Set timer one shot or periodic post event to task          -
  * @param  tId : Task identity                                        -
  * @param  evSig : Signal of event structure                          -
  * @param  period : Timer interval                                    -
  * @param  isReload : One shot type -> false, periodic type -> true   -
  * @retval EventTimerType_t                                           -
  * -------------------------------------------------------------------*/
EventTimerType_t freeAssistantSetTimer(uint8_t tId, uint8_t evSig, uint32_t period, bool isReload) {
    EventTimerType_t softTimId = NULL;
    EventStruct_t *evtTim = NULL;

    faASSERT(tId < virtualAssistant.nbrOfTasks);

    evtTim = (EventStruct_t*)pvPortMalloc(sizeof(EventStruct_t));
    faASSERT(evtTim != NULL);

    evtTim->tId = tId;
    evtTim->evSig = evSig;
    softTimId = xTimerCreate("SoftTimerEvent",
                                    pdMS_TO_TICKS(period),
                                    (isReload) ? pdTRUE : pdFALSE,
                                    (void*)evtTim,
                                    internalSoftTimerCb
                                    );

    faASSERT(NULL != softTimId);                                
    xTimerStart(softTimId, pdMS_TO_TICKS(100));

   return softTimId;
}

/** --------------------------------------------------------------------
  * @brief  Remove software timer event                                -
  * @param  tId : Task identity                                        -
  * @param  evSig : Signal of event structure                          -
  * @param  period : Timer interval                                    -
  * @param  isReload : One shot type -> false, periodic type -> true   -
  * @retval EventTimerType_t                                           -
  * -------------------------------------------------------------------*/
void freeAssistantRemoveTimer(EventTimerType_t mEvtTimId) {
	EventStruct_t *evtTim = (EventStruct_t*)pvTimerGetTimerID(mEvtTimId);

	if (evtTim != NULL) {
		vPortFree(evtTim);
	}

	faASSERT(pdPASS == xTimerDelete(mEvtTimId, pdMS_TO_TICKS(100)));
}

/* Assert function implementation -------------------------------------------*/
void freeAssistantAssert(uint8_t assert, const char *file, uint32_t line) {
    if (!assert) {
        faENTRY_ATOMIC();
        do {
            faLOG(TAG, "[ASSERT]\r\n");
            faLOG(TAG, " -File: %s\r\n", file);
            faLOG(TAG, " -Line: %ld\r\n", line);

            ledLifeBlink();
            vTaskDelay(200);
        } while(faASSERT_VAL);

		softReset();
    }
}

/* Private function implementation -------------------------------------------*/
void FreeRTOSAssistantIntro() {
    faBANNER("\r\n"
            " __ \\                                           \r\n"
            " |   |  |   |  __ \\    _` |  __ `__ \\    _ \\  \r\n"
            " |   |  |   |  |   |  (   |  |   |   |  (   |    \r\n"
            "____/  \\__, | _|  _| \\__,_| _|  _|  _| \\___/  \r\n"
            "       ____/                                     \r\n"
            );

    faBANNER("FreeRTOS Assistant %s\r\n", faVERSION);
    faBANNER(" -Bot: %s\r\n", faASSISTANT_NAME);
    faBANNER(" -Run as: %s\r\n", faBUILD);
    faBANNER(" -Date: %s, %s\r\n\r\n", __DATE__, __TIME__);
}

void internalSoftTimerCb(TimerHandle_t timId) {
    EventStruct_t *evtTimer = (EventStruct_t*)pvTimerGetTimerID(timId);
    faASSERT(evtTimer != NULL);

    freeAssistantPostEvent(evtTimer->tId, evtTimer->evSig, NULL, 0);
}



                              
