#pragma once
#include "device/GPS.h"

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <driver/uart.h>

using namespace device;

namespace support {
    class GpsNmea : public Gps {
        private:
            const uart_port_t uartPort;
            const gpio_num_t rxPin;
            bool initiliazed;
            nmea_parser_handle_t nmeaHandler;
            gps_t lastKnownLocation;
            EventGroupHandle_t eventGroup;        

        public:
            GpsNmea(uart_port_t uartPort, gpio_num_t rxPin);
            void init() override;
            const gps_t& senseLastKnownLocation() override;
            void deinit() override;
        private: 
            void addLocationUpdateHandler(esp_event_handler_t handler, void* eventArg);
            void removeLocationUpdateHandler(esp_event_handler_t handler);
            static void gpsUpdateHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    };
};