#pragma once

#include "Modem.h"
#include "DCE.h"
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"
#include "lwip/dns.h"
#include "tcpip_adapter.h"
#include "esp_log.h"

namespace modem {
    struct Sim800Config {
        const char* apn;
        const char* username;
        const char* password;
        gpio_num_t resetPin;
        gpio_num_t powerKeyPin;
        gpio_num_t powerOnPin;
    };

    class Sim800 : public Modem {
        private:
            Sim800Config config;
            dce::DCE& dce;
            esp_event_loop_handle_t eventloop;
            ppp_pcb* ppp;
            netif pppif;
            
        public:
            Sim800(Sim800Config simConfig, dce::DCE& dce);
            bool start() override;
            bool connect() override;
            bool disconnect() override;
            void registerEventHandler(ModemEventHandler eventHandler, Context ctx) override;
            void unregisterEventHandler(ModemEventHandler eventHandler) override;
            
            bool enablePPP(const char* apn);
            bool enableCommand();

        private:
            void turnOnModule();
            static uint32_t pppOutputHandler(ppp_pcb *pcb, uint8_t *data, uint32_t len, void *ctx);
            static void pppStatusChangeHandler(ppp_pcb *pcb, int err_code, void *ctx);
    };
}