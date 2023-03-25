#ifndef __SYS_CTL_H
#define __SYS_CTL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_task_wdt.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#define SetBits(a, b)           (a |= (1 << b))
#define ResetBits(a, b)         (a &= (~(1 << b)))
#define GetBits(a, b)           ((a >> b) & 0x01)

enum eStaNetConnStatus {
    staWIFI_CONN_UNKNOWN = 0,
    staWIFI_CONN_FAILED,
    staWIFI_CONN_SUCCESS,
    staWIFI_CONN_TIMEOUT,
};

typedef void (*pf_netConnectedCb)(void);

typedef struct t_EspNetworkManager {
    struct {
        uint8_t staStt;
        char staIpAddr[15];
        pf_netConnectedCb staNetConnectedCb;
    } staParams;

    struct {
        char ssId[10];
        char pass[10];
        char apIpAddr[15];
        pf_netConnectedCb apNetConnectedCb;
    } apParams;
} espNetManager_t;

extern void watchdogInit(uint32_t timeOut);
extern void watchdogRst();
extern void softReset();
extern char *getRstReason();

extern uint8_t staNetInit(char *ssid, char *pass, pf_netConnectedCb staConnectedCb);
extern uint8_t staNetStatus();
extern void apNetInit(char *ssid, char *pass, pf_netConnectedCb apConnectedCb);
extern char *staNetGetIp();
extern char *apNetGetIp();

extern void dsnChangeUriToHostIp(const char *strIn, char *strOut);

#ifdef __cplusplus
}
#endif

#endif