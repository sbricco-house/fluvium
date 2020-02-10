#pragma once
#include "device/DistanceSensor.h"
#include "Metric.h"
#include "Util.h"
#include <driver/gpio.h>
#include <freertos/event_groups.h>
#include <freertos/FreeRTOS.h>
using namespace device;
namespace support {
    class Sonar : public DistanceSensor {
        #define RESET_TIME 2
        #define TRIGGER_SIGNAL_TIME 10
        #define SOUND_VEL 331.3
        #define TEMP_RELATION 0.606
        #define ESP_INTR_FLAG_DEFAULT 0
        private:
            gpio_num_t trigPin;
            gpio_num_t echoPin;
            metric::meter maxDistance;
            timeutils::microsecond currentTime;
            float timeoutMeasurement;
            timeutils::second deltaTime;
            EventGroupHandle_t group; //used to synchronize echo phases.
        public:
            Sonar(gpio_num_t trigPin,
                gpio_num_t echoPin,
                metric::meter maxDistance = 6.0,
                timeutils::millisecond maxTimeout = 240);
            void init() override;
            metric::meter senseDistance(metric::celsius temperature) override;
        private:
            double computeSoundVelocity(metric::celsius temp);
            //ISR ROUTINE
            static void SONAR_ECHO(void* arg);
    };
}