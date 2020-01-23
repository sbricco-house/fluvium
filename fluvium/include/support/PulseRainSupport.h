#pragma once

#include "device/RainGauge.h"
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "Util.h"
#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
namespace support {
    //TODO create a support to manage interrupts
    #define ESP_INTR_FLAG_DEFAULT 0
    class PulseRain : public device::RainGauge {
        #define BOUNDING_THR 300
        //ATTRIBUTES
        private:
            portMUX_TYPE mutex;
            metric::millimeter rainQuantity;
            timeutils::millisecond lastSensed;
            gpio_num_t pulsablePin;
            metric::millimeter delta;
            SemaphoreHandle_t semaphore;
        public:
            PulseRain(gpio_num_t pulsablePin, metric::millimeter deltaQuantity);
            void init() override;
            metric::millimeter getRainQuantity() override;
            void reset() override ;
            metric::millimeter getAndResetQuantity() override;
        private:
            static void PULSE_REC(void* arg);
    };
}