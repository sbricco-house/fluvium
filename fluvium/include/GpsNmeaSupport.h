#pragma once
#include "Gps.h"

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
            GpsNmea(uart_port_t uartPort, gpio_num_t rxPin) : 
                uartPort(uartPort), 
                rxPin(rxPin), 
                initiliazed(false),
                eventGroup(xEventGroupCreate()) {};

            void init() override {
                if(!initiliazed) {
                    initiliazed = true;    
                    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
                    config.uart.rx_pin = rxPin;
                    config.uart.uart_port = uartPort;
                    nmeaHandler = nmea_parser_init(&config);
                    addLocationUpdateHandler(gpsUpdateHandler, this);
                }
            }

            const gps_t& senseLastKnownLocation() override {
                xEventGroupWaitBits(eventGroup, BIT0, pdTRUE, pdTRUE, portMAX_DELAY);
                return lastKnownLocation;
            }

            void deinit() override {
                if(initiliazed) {
                    initiliazed = false;
                    removeLocationUpdateHandler(gpsUpdateHandler);
                    nmea_parser_deinit(nmeaHandler);
                }
            }

        private: 
            void addLocationUpdateHandler(esp_event_handler_t handler, void* eventArg) {
                nmea_parser_add_handler(nmeaHandler, handler, eventArg);
            }

            void removeLocationUpdateHandler(esp_event_handler_t handler) {
                nmea_parser_remove_handler(nmeaHandler, handler);
            }

            static void gpsUpdateHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
                GpsNmea* gpsDevice = (GpsNmea*) event_handler_arg;
                switch (event_id)
                {
                    case GPS_UPDATE: {
                        gps_t* gps = (gps_t*) event_data;
                        gpsDevice->lastKnownLocation = *gps; // implict copy
                        xEventGroupSetBits(gpsDevice->eventGroup, BIT0);
                        break;
                    }
                    case GPS_UNKNOWN: /* ignored */ break;
                }
            }
    };
};