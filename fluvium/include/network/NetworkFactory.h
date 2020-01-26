#pragma once
#include "Gsm.h"

namespace networkfactory {
    static network::Gsm createGsmTTGO(const char* apn) {
        esp_modem_dte_config_t dteConfig = ESP_MODEM_DTE_DEFAULT_CONFIG();
        dteConfig.port_num = UART_NUM_1;
        dteConfig.rx_pin = GPIO_NUM_26;
        dteConfig.tx_pin = GPIO_NUM_27;

        esp_ppp_config_t pppConfig = ESP_MODEM_PPP_DEFAULT_CONFIG(apn);

        /*esp_ppp_config_t pppConfig = {
            .apn_name = apn,
            .ppp_auth_username = "",
            .ppp_auth_password = ""
        };*/

        network::GsmConfig gsmConfig = { 
            .dteConfig = dteConfig, 
            .pppConfig = pppConfig, 
            .resetPin = GPIO_NUM_5, 
            .powerKeyPin = GPIO_NUM_4, 
            .powerOnPin = GPIO_NUM_23
        };

        return network::Gsm(gsmConfig);
    }
}