#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "network/Wifi.h"

const int CONNECTED_BIT = BIT0;
const int DISCONNECTED_BIT = BIT1;

using namespace network;

esp_err_t Wifi::wifiEventHandler(void * arg, system_event_t *event) {
    Wifi * self = (Wifi *) arg; 
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(self->wifiEventGroup, CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            /* This is a workaround as ESP32 WiFi libs don't currently
            auto-reassociate. */
            self->state = DISCONNECTED;
            esp_wifi_connect();
            xEventGroupSetBits(self->wifiEventGroup, DISCONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            self->state = CONNECTED;
            
        default:
            break;
        }
    return ESP_OK;
}
Wifi::Wifi(char * ssid, char * password) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    state = DISCONNECTED;
    tcpip_adapter_init(); //init tcp stack
    wifiEventGroup = xEventGroupCreate(); //event group used to synchronization
    ESP_ERROR_CHECK(esp_event_loop_init(this->wifiEventHandler, this) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

    wifi_config_t wifi_config = {};
    memcpy(wifi_config.sta.ssid, ssid, strlen(ssid) + 1);
    memcpy(wifi_config.sta.password, password, strlen(password) + 1);
    
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


bool Wifi::connect() {
    if(state == CONNECTED) {
        return true;
    }

    esp_wifi_connect();
    xEventGroupWaitBits(wifiEventGroup, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    return state == CONNECTED;
} 
bool Wifi::disconnect() {
    if(state == DISCONNECTED) {
        return true;
    }
    esp_wifi_disconnect();
    
    xEventGroupWaitBits(wifiEventGroup, DISCONNECTED_BIT,
                        false, true, portMAX_DELAY);
    return state == DISCONNECTED;
}
bool Wifi::standby() {
    state = STANDBY;
    return true;
}

NetworkState Wifi::getState() {
    return state;
}