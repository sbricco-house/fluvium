#pragma once
#include "Gsm.h"
#include "SerialDCE.h"
#include "Sim800.h"
#include <memory>

namespace networkfactory {
    static network::Gsm createGsmTTGO(const char* apn) {
        using namespace modem;
        using namespace serial;

        SerialConfig serialConfig = {
            .uartPort = UART_NUM_1,
            .rxPin = GPIO_NUM_26,
            .txPin = GPIO_NUM_27,
            .baudrate = 115200
        };

        Sim800Config modemConfig = {
            .apn = apn,
            .username = "",
            .password = "",
            .resetPin = GPIO_NUM_5, 
            .powerKeyPin = GPIO_NUM_4, 
            .powerOnPin = GPIO_NUM_23
        };

        auto dce = new serial::SerialDCE(serialConfig);
        auto modem = new modem::Sim800(modemConfig, *dce);
        return network::Gsm(*modem);
    }
}