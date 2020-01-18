#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include "Buffering.h"
#include "Task.h"
#include "nmea_parser.h"
#include <driver/uart.h>

// TODO: consider only valid gps position could generate an
// infinite waiting time, consider to add timeout
// TODO: add timestamp to location provided by satellites

namespace buffering {
    #define gpsType 2
    namespace data {
        struct LocationData : public Data {
            double latitude;
            double longitude;
            double altitude;
            float hdop;
            LocationData(double latitude, double longitude, double altitude, float hdop) :
                Data(gpsType),
                latitude(latitude),
                longitude(longitude),
                altitude(altitude),
                hdop(hdop) {};
        };
    }

    namespace parser {
        class LocationParser : public Parser {
            private:
                static const short SERIALIZE_BUFFER = 60;
                short decimalPrecision;
            public:
                LocationParser(short decimalPrecision = 6) : Parser(gpsType) {};
            protected:
                csv doSerialize(const Data& data) {
                    if(data.id != gpsType) {
                        return "";
                    }
                    char buff[SERIALIZE_BUFFER];
                    data::LocationData* location = (data::LocationData*) &data;
                    sprintf(buff, "%ld;%.7lf;%.7lf;%.7lf;%.3f", 
                        location->timestap,
                        location->latitude,
                        location->longitude,
                        location->altitude,
                        location->hdop);
                    
                    return csv(buff);
                }
        };
    }
}

namespace device {

    class SerialNmeaGps {
        private:
            const uart_port_t uartPort;
            const gpio_num_t rxPin;
            nmea_parser_handle_t nmeaHandler;
            gps_t lastKnownLocation;
            EventGroupHandle_t eventGroup;
            bool started;
            static void gpsUpdateHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);            

        public:
            SerialNmeaGps(uart_port_t uartPort, gpio_num_t rxPin);
            bool requireLocationUpdate();
            bool stopLocationUpdate();
            void addLocationUpdateHandler(esp_event_handler_t handler, void* eventArg);
            void removeLocationUpdateHandler(esp_event_handler_t handler);
            /* Blocking method for obtain the last know location */
            const gps_t& getLastKnownLocation();
    };

    SerialNmeaGps::SerialNmeaGps(uart_port_t uartPort, gpio_num_t rxPin) :
        uartPort(uartPort), rxPin(rxPin), eventGroup(xEventGroupCreate()), started(false) {};

    bool SerialNmeaGps::requireLocationUpdate() {
        if(!started) {
            started = true;    
            nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
            config.uart.rx_pin = rxPin;
            config.uart.uart_port = uartPort;
            nmeaHandler = nmea_parser_init(&config);
            addLocationUpdateHandler(gpsUpdateHandler, this);
        }
        return true;
    }

    bool SerialNmeaGps::stopLocationUpdate() {
        if(started) {
            started = false;
            removeLocationUpdateHandler(gpsUpdateHandler);
            nmea_parser_deinit(nmeaHandler);
        }
        return true;
    }

    void SerialNmeaGps::addLocationUpdateHandler(esp_event_handler_t handler, void* eventArg) {
        nmea_parser_add_handler(nmeaHandler, handler, eventArg);
    }

    void SerialNmeaGps::removeLocationUpdateHandler(esp_event_handler_t handler) {
        nmea_parser_remove_handler(nmeaHandler, handler);
    }

    const gps_t& SerialNmeaGps::getLastKnownLocation() {
        xEventGroupWaitBits(eventGroup, BIT0, pdTRUE, pdTRUE, portMAX_DELAY);
        return lastKnownLocation;
    }

    void SerialNmeaGps::gpsUpdateHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
        SerialNmeaGps* gpsDevice = (SerialNmeaGps*) event_handler_arg;
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

namespace task {
    using namespace buffering::data;
    
    class LocationTask : public Task {
        private:
            static const int TARGET_NUMBER_SAMPLES = 10;
            static const short DELAY_MS_FOR_FIX = 10000; // 10 seconds 
            device::SerialNmeaGps& gps;         

        public:
            LocationTask(const Buffer& buffer, device::SerialNmeaGps& gps) : 
                Task(buffer),
                gps(gps){};

            void run() override {
                LocationData* currentBest = nullptr;
                // some delay for wait a gps fix
                vTaskDelay(pdMS_TO_TICKS(DELAY_MS_FOR_FIX));

                gps.requireLocationUpdate();
                
                for(int i = 0; i < TARGET_NUMBER_SAMPLES; ) {   
                    gps_t lastKnownLocation = gps.getLastKnownLocation();
                    // only valid location are considered valid sample
                    if(lastKnownLocation.valid) {
                        if(currentBest == nullptr) {
                            currentBest = locationDataFromNmea(lastKnownLocation);
                        }
                        else if(lastKnownLocation.dop_h < currentBest->hdop) {
                            delete currentBest;
                            currentBest = locationDataFromNmea(lastKnownLocation);
                        } 
                        i++;
                    }
                }
                gps.stopLocationUpdate();

                printf("Latitude: %lf\nLongitude: %lf\nAltitude: %lf\nHDop: %lf\n",
                    currentBest->latitude,
                    currentBest->longitude,
                    currentBest->altitude,
                    currentBest->hdop);

                buffer.queue(currentBest);
            };
        private:
            static LocationData* locationDataFromNmea(gps_t gps) {
                return new LocationData {
                    gps.latitude,
                    gps.longitude,
                    gps.altitude,
                    gps.dop_h
                };
            }
    };
}