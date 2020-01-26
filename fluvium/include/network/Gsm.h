#pragma once

#include "Network.h"
#include "freertos/event_groups.h"
#include "modem.h"

namespace network {    

    struct GsmConfig {
        esp_modem_dte_config_t dteConfig;
        esp_ppp_config_t pppConfig;
        gpio_num_t resetPin;
        gpio_num_t powerKeyPin;
        gpio_num_t powerOnPin;
    };

    class Gsm : public Network {
        private:
            GsmConfig config;
            const EventGroupHandle_t eventGroup;
            modem_dte_t* dteHandler;
            modem_dce_t* dceHandler;

        public:
            Gsm(const GsmConfig config);
            ~Gsm();
            NetworkState getState() override;
            bool connect() override;
            bool disconnect() override;
            bool standby() override;
        private:
            static void modemEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            bool initModem();
            void powerOnModem();
            bool startConnect();
            bool waitConnectionDone();
            bool startDisconnect();
            bool waitDisconnectionDone();
    };
}