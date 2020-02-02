#pragma once

#include "Network.h"
#include "Modem.h"
#include "freertos/event_groups.h"

namespace network {    

    class Gsm : public Network {
        private:
            modem::Modem& modem;
            const EventGroupHandle_t eventGroup;

        public:
            Gsm(modem::Modem& modem);
            ~Gsm();
            NetworkState getState() override;
            bool connect() override;
            bool disconnect() override;
            bool standby() override;

        private:
            static void modemEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            bool initModem();
            bool startConnect();
            bool waitConnectionDone();
            bool startDisconnect();
            bool waitDisconnectionDone();
    };
}