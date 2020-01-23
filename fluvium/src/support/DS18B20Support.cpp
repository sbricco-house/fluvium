#include "support/DS18B20Support.h"

#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "device/Device.h"
#include "Metric.h"
#include "device/TemperatureSensor.h"
using namespace device;
using namespace support;
//PRIVATE
void DS18B20::sendBit(uint8_t bit) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, LOW);
    ets_delay_us(5);
    if(bit == HIGH) {
        gpio_set_level(pin,HIGH);
    }
    ets_delay_us(80);
    gpio_set_level(pin,HIGH);
}
uint8_t DS18B20::readBit(void) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin,LOW);
    ets_delay_us(2);
    gpio_set_level(pin,HIGH);
    ets_delay_us(15);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    return gpio_get_level(pin) == HIGH ? HIGH : LOW; //bit presence
}
void DS18B20::sendByte(uint8_t data) {
    uint8_t i;
    uint8_t bit;
    for(i = 0; i < BYTE_LEN; i++){
        bit = data>>i; //move bit at the first position
        bit &= BIT0; //take the value in the position
        sendBit(bit);
    }
    ets_delay_us(100);
}
uint8_t DS18B20::readByte(void) { 
    uint8_t i;
    uint8_t data = 0;
    for (i = 0; i < BYTE_LEN; i++)
    {
        if(readBit()) {
            data |= BIT0 << i; // put high the current bit.
        } 
        ets_delay_us(15);
    }
    return(data);
}
bool DS18B20::resetPulse(void) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, LOW);
    ets_delay_us(500);
    gpio_set_level(pin, HIGH);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    ets_delay_us(30);
    gpio_get_level(pin);
    ets_delay_us(470);
    return gpio_get_level(pin) == HIGH;
}
void DS18B20::select() {
    sendByte(SELECT);
}
void DS18B20::selectAndExec(OneWireCommand command) {
    select();
    sendByte(command);
}
void DS18B20::sendZeros(int zeros) {
    for(int i = 0; i < zeros; i++) {
        sendByte(0x00);
    }
}
//PUBLIC
DS18B20::DS18B20(gpio_num_t pin, Precision precision) : pin(pin), precision(precision) {}

void DS18B20::init() {
    gpio_pad_select_gpio(pin);
    selectAndExec(CONFIG);
    //put configuration
    sendZeros(2); //first two byte in memory is used to store used bytes, in this case they are be ignored.
    sendByte(precision.mask); //change precision
    resetPulse();
}
metric::celsius DS18B20::senseTemperature() {
    uint8_t tempByte1, tempByte2;
    if(resetPulse()) {
        //send command to read temperature
        selectAndExec(COMPUTE);
        //wait temperature computation..
        vTaskDelay(pdMS_TO_TICKS(precision.timeout));
        resetPulse();
        //read temperature from sensor
        selectAndExec(READ);
        tempByte1=readByte();
        tempByte2=readByte();
        //Import! used to manage negative value.
        int16_t temperature = (tempByte1 + SECOND_BYTE_VALUE(tempByte2));
        resetPulse();
        return temperature / 16.0;
    } else {
        return 0;
    }
}