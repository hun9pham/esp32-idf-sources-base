#include "mqtt_conn.h"

#define TAG "MQTTConn"

static esp_mqtt_client_handle_t clientManagerConn;
static volatile uint8_t mqttConnStatus = CONN_UNKNOWN;

static void mqttEventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    (void)handler_args;
    (void)base;

    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_BEFORE_CONNECT: {
    SYS_LOG(TAG, "MQTT_EVENT_BEFORE_CONNECT");
    
    /*
        - TODO
        -
    */
    mqttConnStatus = CONN_DISCONNECTED;
    }
    break;

    case MQTT_EVENT_CONNECTED: {
        SYS_LOG(TAG, "MQTT_EVENT_CONNECTED");
        
        /*
         - TODO
         -
        */
        mqttConnStatus = CONN_CONNECTED;
    }
    break;

    case MQTT_EVENT_DISCONNECTED: {
        SYS_LOG(TAG, "MQTT_EVENT_DISCONNECTED");

        /*
         - TODO
         -
        */
        mqttConnStatus = CONN_DISCONNECTED;
    }
    break;

    case MQTT_EVENT_SUBSCRIBED: {
        SYS_LOG(TAG, "MQTT_EVENT_SUBSCRIBED");
        
        /*
         - TODO
         -
        */
    }
    break;

    case MQTT_EVENT_UNSUBSCRIBED: {
        SYS_LOG(TAG, "MQTT_EVENT_UNSUBSCRIBED");

        /*
         - TODO
         -
        */
    }
    break;

    case MQTT_EVENT_PUBLISHED: {
        SYS_LOG(TAG, "MQTT_EVENT_PUBLISHED");

        /*
         - TODO
         -
        */
    }
    break;

    case MQTT_EVENT_DATA: {
        SYS_LOG(TAG, "MQTT_EVENT_DATA");
        SYS_LOG(TAG, "TOPIC  : %.*s", event->topic_len, event->topic);
        SYS_LOG(TAG, "DATA   : %.*s", event->data_len, event->data);
        SYS_LOG(TAG, "LENGTH : %d", event->data_len);
        
        /*
         - TODO
         -
         */
    }    
    break;
    case MQTT_EVENT_ERROR: {
        SYS_LOG(TAG, "MQTT_EVENT_ERROR");

        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            SYS_LOG(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            SYS_LOG(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            SYS_LOG(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            SYS_LOG(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            SYS_LOG(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
    }
    break;

    default: {
        SYS_LOG(TAG, "Other event id:%d", event->event_id);
    }
    break;
    }
}

void mqttConnInit(mqttClientSett_t *paramClient) {
    char uriStr[MQTT_URI_MAX_LEN];

    memset(uriStr, 0, MQTT_URI_MAX_LEN);

#if DSN_RESOLVE_HOST_EN
    char ipStr[MQTT_HOST_IP_LEN];

    dsnChangeUriToHostIp(paramClient->uriBroker, ipStr);
    sprintf(uriStr, "mqtt://%s", ipStr);
#else
    sprintf(uriStr, "mqtt://%s", paramClient->uriBroker);
#endif

    SYS_LOG(TAG, "-URI: %s", paramClient->uriBroker);
    SYS_LOG(TAG, "-IP : %s", uriStr);

    const esp_mqtt_client_config_t mqttConfigure = {
        .broker = {
            .address.uri = uriStr,
            .address.port = paramClient->portNum,
        },
    };

    clientManagerConn = esp_mqtt_client_init(&mqttConfigure);

    /* The last argument may be used to pass data to the event handler */
    esp_mqtt_client_register_event(clientManagerConn, paramClient->tId, mqttEventHandler, NULL);
    esp_mqtt_client_start(clientManagerConn);
}

void mqttConnReconnect() {
    esp_mqtt_client_reconnect(clientManagerConn);
}

void mqttConnStop() {
    esp_mqtt_client_stop(clientManagerConn);
}

int8_t mqttSubcribe(TopicType_t paramTopic, eQoSType_t paramQoS) {
    return (esp_mqtt_client_subscribe(clientManagerConn, paramTopic, paramQoS) > 0 ? MQTT_CONN_OK : MQTT_CONN_NG);
}

int8_t mqttUnsubcribe(TopicType_t paramTopic, eQoSType_t paramQoS) {
    return (esp_mqtt_client_unsubscribe(clientManagerConn, paramTopic) > 0 ? MQTT_CONN_OK : MQTT_CONN_NG);
}

int8_t mqttPublish(TopicType_t paramTopic, char *payload, uint32_t len, eQoSType_t paramQoS, bool isRetain) {
    return (esp_mqtt_client_publish(clientManagerConn, paramTopic, (const char*)payload, len, paramQoS, isRetain) > 0 ? MQTT_CONN_OK : MQTT_CONN_NG);
}

uint8_t mqttConnStatusGet() {
    return mqttConnStatus;
}