#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <setjmp.h>
#include <unistd.h>

#include "sdkconfig.h"

#include "sys_log.h"
#include "sys_soft.h"

#define TAG "SysSoft"

static espNetManager_t espNetIf;

void watchdogInit(uint32_t timeOut) { 
    /* Require in sdkconfig: 
        # CONFIG_ESP_TASK_WDT_INIT is not set
    */
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = timeOut,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = false,
    };

    ESP_ERROR_CHECK(esp_task_wdt_init(&twdt_config));
}

void watchdogRst() {
    static bool takenBy = false;

    if (takenBy == false) {
        esp_task_wdt_add(NULL);
        esp_task_wdt_status(NULL);
        takenBy = true;
    }
    esp_task_wdt_reset();
}

void softReset() {
    fflush(stdout);
    esp_restart();
}

char * getRstReason() {
    esp_reset_reason_t resId = esp_reset_reason();
    char *res = "UNK";

    switch (resId) {
    case ESP_RST_UNKNOWN:
    break;

    case ESP_RST_POWERON:
        res = "PWR_ON";
    break;

    case ESP_RST_EXT:
        res = "EXT_PIN";
    break;

    case ESP_RST_SW:
        res = "SOFT";
    break;

    case ESP_RST_PANIC:
        res = "PANIC";
    break;

    case ESP_RST_INT_WDT:
        res = "IWDT";
    break;

    case ESP_RST_TASK_WDT:
        res = "TWDT";
    break;

    case ESP_RST_WDT:
        res = "WDT";
    break;

    case ESP_RST_DEEPSLEEP:
        res = "DEEPSLEEP";
    break;

    case ESP_RST_BROWNOUT:
        res = "BROWNOUT";
    break;

    case ESP_RST_SDIO:
        res = "SDIO";
    break;

    default:
    break;
    };

    return res;
}

static void staEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static uint8_t retryCnt = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (retryCnt < 5) {
            esp_wifi_connect();
            retryCnt++;
            SYS_LOG(TAG, "Reconnect ...");
        } 
        else {
            espNetIf.staParams.staStt = staWIFI_CONN_FAILED;
        }
        SYS_LOG(TAG, "ESP STA ENTRY WIFI [NG]");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        esp_ip4addr_ntoa(&event->ip_info.ip, espNetIf.staParams.staIpAddr, 15);
        SYS_LOG(TAG, "ESP STA ENTRY WIFI [OK]");
        SYS_LOG(TAG, "IP ADDR: %s", espNetIf.staParams.staIpAddr);
        retryCnt = 0;
        espNetIf.staParams.staStt = staWIFI_CONN_SUCCESS;
        if (espNetIf.staParams.staNetConnectedCb != NULL) {
            espNetIf.staParams.staNetConnectedCb();
        }
    }
}

uint8_t staNetInit(char *ssid, char *pass, pf_netConnectedCb staConnectedCb) {
    uint8_t ret = staWIFI_CONN_UNKNOWN;
    uint16_t timeOut = 0xFFFF;

    espNetIf.staParams.staStt = staWIFI_CONN_UNKNOWN;
    espNetIf.staParams.staNetConnectedCb = staConnectedCb;
    memset(espNetIf.staParams.staIpAddr, 0, 15 * sizeof(char));

    esp_err_t nvsRet = nvs_flash_init();
    if (nvsRet == ESP_ERR_NVS_NO_FREE_PAGES || nvsRet == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      nvsRet = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvsRet);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &staEventHandler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &staEventHandler,
                                                        NULL,
                                                        &instance_got_ip));
    
    wifi_config_t wifi_config = {
        .sta = {
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
	         * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    strcpy((char*)&wifi_config.sta.ssid[0], ssid);
    strcpy((char*)&wifi_config.sta.password[0], pass);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    do {
        if (espNetIf.staParams.staStt != staWIFI_CONN_UNKNOWN) {
            break;
        }
    } while (timeOut-- > 0);

    if (timeOut == 0) {
        ret = staWIFI_CONN_TIMEOUT;
    }
    else {
        ret = espNetIf.staParams.staStt;
    }

    return ret;
}

static void apEventHandler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)event_data;

        SYS_LOG(TAG, "DEVICE CONNECTS AP NETWORK\r\n -MAC: %x:%x:%x:%x:%x:%x\r\n -ID: %d", 
            event->mac[0],
            event->mac[1],
            event->mac[2],
            event->mac[3],
            event->mac[4],
            event->mac[5],
            event->aid
            );

        if (espNetIf.apParams.apNetConnectedCb != NULL) {
            espNetIf.apParams.apNetConnectedCb();
        }
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)event_data;

        SYS_LOG(TAG, "DEVICE DISCONNECTS AP NETWORK\r\n -MAC: %x:%x:%x:%x:%x:%x\r\n -ID: %d", 
            event->mac[0],
            event->mac[1],
            event->mac[2],
            event->mac[3],
            event->mac[4],
            event->mac[5],
            event->aid
            );
    }
}

void apNetInit(char *ssid, char *pass, pf_netConnectedCb apConnectedCb) {
    memset(espNetIf.apParams.apIpAddr, 0, 15 * sizeof(char));
    strcpy((char*)&espNetIf.apParams.ssId, ssid);
    strcpy((char*)&espNetIf.apParams.pass, pass);
    espNetIf.apParams.apNetConnectedCb = apConnectedCb;

    esp_err_t nvsRet = nvs_flash_init();
    if (nvsRet == ESP_ERR_NVS_NO_FREE_PAGES || nvsRet == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      nvsRet = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvsRet);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t * p_netif = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &apEventHandler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid_len = strlen(ssid),
            .channel = 1, /* Default channel */
            .max_connection = 5,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
    };
    strcpy((char*)&wifi_config.ap.ssid[0], ssid);
    strcpy((char*)&wifi_config.ap.password[0], pass);

    if (strlen(ssid) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t apNetIpAddr;
    ESP_ERROR_CHECK(esp_netif_get_ip_info(p_netif, &apNetIpAddr));
    esp_ip4addr_ntoa(&apNetIpAddr.ip, espNetIf.apParams.apIpAddr, 15);

    SYS_LOG(TAG, "ESP AP START\r\n -SSID: %s\r\n -PASS: %s\r\n -IP ADDR: %s", 
            espNetIf.apParams.ssId, 
            espNetIf.apParams.pass, 
            espNetIf.apParams.apIpAddr
            );
}

char *staNetGetIp() {
    char *retIp = NULL;

    if (espNetIf.staParams.staStt == staWIFI_CONN_SUCCESS) {
        retIp = espNetIf.staParams.staIpAddr; 
    }

    return retIp;
}

char *apNetGetIp() {
    char *retIp = NULL;

    retIp = espNetIf.apParams.apIpAddr; 
    
    return retIp;
}

uint8_t staNetStatus() {
    return espNetIf.staParams.staStt;
}


static void dsnFoundCb(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    if (ipaddr != NULL) {
        SYS_LOG(TAG, "dsnFoundCb: Success");
        strcpy((char*)arg, ipaddr_ntoa(ipaddr));
    }
    else {
        SYS_LOG(TAG, "dsnFoundCb: Failed");
    }
}

void dsnChangeUriToHostIp(const char *strIn, char *strOut) {
    ip_addr_t addrIp;
    dns_gethostbyname(strIn, &addrIp, dsnFoundCb, (void*)strOut);
}
