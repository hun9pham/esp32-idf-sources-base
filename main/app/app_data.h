#ifndef __APP_DATA_H
#define __APP_DATA_H

#include <stdio.h>
#include <stdint.h>

#define adtSTA_SSID	           (char*)"Tien"
#define adtSTA_PASS	           (char*)"0348799826"

#define adtAP_SSID             (char*)"ESP-AP"
#define adtAP_PASS             (char*)"espap@123"

#define adtMQTT_PUB_TOPIC      (char*)"esp/uplink"
#define adtMQTT_SUB_TOPIC      (char*)"esp/downlink"

#define adtMQTT_BROKER_URI     (const char *)"broker.hivemq.com"
/* #define adtMQTT_BROKER_URI     (const char *)"192.168.1.5" */
#define adtMQTT_BROKER_PORT    ( 1883 )
#define adtMQTT_USERNAME       ( NULL )
#define adtMQTT_PASSWORD       ( NULL )

#endif
