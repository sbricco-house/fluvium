#pragma once

#include "device/SoilMoistureSensor.h"
#include <driver/gpio.h>
#include <driver/adc.h>

using namespace device;

namespace support {
    class SparkFunMoisture : public SoilMoistureSensor {
        #define MAX_INPUT_VOLTAGE 2.2
        private:
            static const adc_atten_t ATTENUATION = ADC_ATTEN_DB_0; // max input 2.2v
            static const short MIN_SOIL_MOISTURE = 0;
            static const short MAX_SOIL_MOISTURE = 100;
            const gpio_num_t powerPin;
            const adc1_channel_t channel;
            const adc_bits_width_t precision;
        public:
            SparkFunMoisture(gpio_num_t powerPin, adc1_channel_t channel, adc_bits_width_t precision);
            void init() override;
            metric::relativehumidity senseSoilMoisture() override;
        private:
            static double mapRange(double x, double inMin, double inMax, double outMin, double outMax);
            static int precisionToMaxReadableValue(adc_bits_width_t width);
    };
}