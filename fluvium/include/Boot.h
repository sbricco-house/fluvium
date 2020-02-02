#pragma once

#include "esp_attr.h"
#include "Ntp.h"
#include "network/Network.h"

RTC_DATA_ATTR static int bootCount = 0;

namespace boot {
    static inline void countBoot() { bootCount++; }
    static inline int getBootCount() { return bootCount; }
    static inline bool isFirstBoot() { return getBootCount() == 1; }
    static inline void setupTimeAtFirstBoot(network::Network& net) {
        while(!net.connect()) { vTaskDelay(pdMS_TO_TICKS(200)); }
        ntputils::setupTime();
    }
}