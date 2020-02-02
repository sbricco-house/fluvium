#pragma once

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(MODEM_EVENT)

#define PPP_START 0
#define PPP_CONNECTED 1
#define PPP_DISCONNECTED 2
#define PPP_STOP 3

using ModemEventHandler = esp_event_handler_t;
using Context = void*;

namespace modem {
    struct ModemIpInfo {
        ip4_addr_t ip;      /*!< IP Address */
        ip4_addr_t netmask; /*!< Net Mask */
        ip4_addr_t gw;      /*!< Gateway */
        ip4_addr_t ns1;     /*!< Name Server1 */
        ip4_addr_t ns2;     /*!< Name Server2 */
    };

    class Modem {
        
        public:
            virtual bool start() = 0;
            virtual bool connect() = 0;
            virtual bool disconnect() = 0;
            virtual void registerEventHandler(ModemEventHandler eventHandler, Context ctx) = 0;
            virtual void unregisterEventHandler(ModemEventHandler eventHandler) = 0;
    };
}