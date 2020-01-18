#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "Device.h"
#include "Metric.h"
#include "TemperatureSensor.h"
using namespace device;
namespace support {
    #define BYTE_LEN 8
    #define MULTIPLIER 256 
    //take only first three bit
    #define SECOND_BYTE_VALUE(value) (value) * MULTIPLIER
    //Look temperature schema to see why i take the third bit from the temperature value
    #define IS_NEGATIVE(value) (value & BIT4) == 1
    
    enum OneWireCommand {
        READ = 0xBE, //read the value in memory
        COMPUTE = 0x44, //ask to digital sensor to compute temperature
        CONFIG = 0x4E, //enter in configuration mode
        SELECT = 0xCC//select the sensor temperature
    };
    /*ds18b20 temperature precision*/
    struct Precision { 
        int mask; //mask used to send to device
        int timeout; //the time used to wait the next temperature
    };
    /**
     * the temperature reading consist in the collection of 
     * two byte form ds18b20 EEPROM structured like this:
     *                                 B7   B6   B5   B4   B3   B2   B1   B0
     *  BYTE 0 (least significamt LS) [2^3 ;2^2 ;2^1 ;2^0 ;2^-1;2^-2;2^-3;2^-4]
     *  BYTE 1 (most significant MS)  [SIGN;SIGN;SIGN;SIGN;SIGN;2^6 ;2^5 ;2^4]
     * the precision tell how many bit are taken fron the left to right.
     */
    const Precision P12 {0x7F, 750}; //12 bit of precision
    const Precision P11 {0x5F, 375}; //11 bit of precision
    const Precision P10 {0x3F, 190}; //10 bit of preciosn
    const Precision P9 {0x1F, 100}; //9 bit of precision
    /**
     * this implementation logic follows the information write in the datasheet: [https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf]
     */
    class DS18B20 : public TemperatureSensor {
        private:
            gpio_num_t pin;
            Precision precision;
            //send single signal to sensor
            void sendBit(uint8_t bit) {
                gpio_set_direction(pin, GPIO_MODE_OUTPUT);
                gpio_set_level(pin, LOW);
                ets_delay_us(5);
                if(bit == HIGH) {
                    gpio_set_level(pin,HIGH);
                }
                ets_delay_us(80);
                gpio_set_level(pin,HIGH);
            }
            //read single signal from sensor
            uint8_t readBit(void) {
                gpio_set_direction(pin, GPIO_MODE_OUTPUT);
                gpio_set_level(pin,LOW);
                ets_delay_us(2);
                gpio_set_level(pin,HIGH);
                ets_delay_us(15);
                gpio_set_direction(pin, GPIO_MODE_INPUT);
                return gpio_get_level(pin) == HIGH ? HIGH : LOW; //bit presence
            }
            //send a single byte to the sensor
            void sendByte(uint8_t data) {
                uint8_t i;
                uint8_t bit;
                for(i = 0; i < BYTE_LEN; i++){
                    bit = data>>i; //move bit at the first position
                    bit &= BIT0; //take the value in the position
                    sendBit(bit);
                }
                ets_delay_us(100);
            }
            //read a single byte from the sensor
            uint8_t readByte(void) { 
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
            //Sends reset pulse to sensor, cancel last command sent
            bool resetPulse(void) {
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
            inline void select() {
                sendByte(SELECT);
            }
            inline void selectAndExec(OneWireCommand command) {
                select();
                sendByte(command);
            }
            inline void sendZeros(int zeros) {
                for(int i = 0; i < zeros; i++) {
                    sendByte(0x00);
                }
            }
        public:
            DS18B20(gpio_num_t pin, Precision precision) : pin(pin), precision(precision) {}

            void init() override {
                gpio_pad_select_gpio(pin);
                selectAndExec(CONFIG);
                //put configuration
                sendZeros(2); //first two byte in memory is used to store used bytes, in this case they are be ignored.
                sendByte(precision.mask); //change precision
                resetPulse();
            }
            metric::celsius senseTemperature() override {
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
    };
}