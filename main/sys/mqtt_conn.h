#ifndef __MQTT_CONN_H
#define __MQTT_CONN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sys_log.h"
#include "sys_soft.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note:
 *----------------------------------------------------------------------------*/
#define MQTT_CONN_OK        ( 1 )
#define MQTT_CONN_NG        ( 0 )
#define MQTT_URI_MAX_LEN    ( 50 )
#define MQTT_HOST_IP_LEN    ( 10 )

/* Typedef -------------------------------------------------------------------*/
typedef char* TopicType_t;

enum eMQTTConnStatus {
    CONN_UNKNOWN = 0,
    CONN_DISCONNECTED,
    CONN_CONNECTED,
};

typedef enum eQualityOfService {
    QoS_MSG_LEVEL0 = 0,
    QoS_MSG_LEVEL1,
    QoS_MSG_LEVEL2
} eQoSType_t;

typedef struct {
    uint16_t portNum;
    const char *uriBroker;
    int32_t tId;
    const char *username;
    const char *password;
} mqttClientSett_t;

/* Extern variables ----------------------------------------------------------*/
extern void mqttConnInit(mqttClientSett_t *paramClient);
extern void mqttConnReconnect(void);
extern void mqttConnStop(void);

extern int8_t mqttSubcribe(TopicType_t paramTopic, eQoSType_t paramQoS);
extern int8_t mqttUnsubcribe(TopicType_t paramTopic, eQoSType_t paramQoS);
extern int8_t mqttPublish(TopicType_t paramTopic, char *payload, uint32_t len, eQoSType_t paramQoS, bool isRetain);

extern uint8_t mqttConnStatusGet(void);

/* Function prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif