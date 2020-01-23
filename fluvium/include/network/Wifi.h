#pragma once
#include "Network.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
namespace network {
    class Wifi : public Network {
        private:
            NetworkState state;
            EventGroupHandle_t wifiEventGroup;
        public:
            Wifi(char * ssid, char * password);
            NetworkState getState() override;
            bool connect() override;
            bool disconnect() override;
            bool standby() override;
        private:
            static esp_err_t wifiEventHandler(void * arg, system_event_t *event);
    };
}