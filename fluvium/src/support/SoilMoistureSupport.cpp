#include "support/SoilMoistureSupport.h"

using namespace support;

SparkFunMoisture::SparkFunMoisture(gpio_num_t powerPin, adc1_channel_t channel, adc_bits_width_t precision)
    : powerPin(powerPin), channel(channel), precision(precision) {};

void SparkFunMoisture::init() {
    gpio_reset_pin(powerPin);
    //set pin direction
    gpio_set_direction(powerPin, GPIO_MODE_OUTPUT);
    // set precision and channel for ADC
    adc1_config_width(precision);
    adc1_config_channel_atten(channel, ATTENUATION);
}
metric::relativehumidity SparkFunMoisture::senseSoilMoisture() {
    gpio_set_level(powerPin, HIGH);
    int value = adc1_get_raw(channel);
    auto soilMoisture = mapRange(value, 0, precisionToMaxReadableValue(precision), MIN_SOIL_MOISTURE, MAX_SOIL_MOISTURE);
    gpio_set_level(powerPin, LOW);
    return soilMoisture;    
}
double SparkFunMoisture::mapRange(double x, double inMin, double inMax, double outMin, double outMax) {
    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}
int SparkFunMoisture::precisionToMaxReadableValue(adc_bits_width_t width) {
    switch(width) {
        case ADC_WIDTH_9Bit:
            return 512;
        case ADC_WIDTH_10Bit:
            return 1024;
        case ADC_WIDTH_11Bit:
            return 2048;
        case ADC_WIDTH_12Bit:
            return 4096;
        default:
            return -1;
    }
}