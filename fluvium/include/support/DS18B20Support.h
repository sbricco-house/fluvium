#pragma once
#include "driver/gpio.h"
#include "device/Device.h"
#include "Metric.h"
#include "device/TemperatureSensor.h"
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
        public:
            DS18B20(gpio_num_t pin, Precision precision);
            void init() override;
            metric::celsius senseTemperature() override;
        private:
            //send single signal to sensor
            inline void sendBit(uint8_t bit);
            //read single signal from sensor
            inline uint8_t readBit(void);
            //send a single byte to the sensor
            inline void sendByte(uint8_t data);
            //read a single byte from the sensor
            inline uint8_t readByte(void);
            //Sends reset pulse to sensor, cancel last command sent
            bool resetPulse(void);
            inline void select();
            inline void selectAndExec(OneWireCommand command);
            inline void sendZeros(int zeros);
        
    };
}