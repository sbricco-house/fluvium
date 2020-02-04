#include "support/GpsNmeaSupport.h"
using namespace support;
//PUBLIC
GpsNmea::GpsNmea(uart_port_t uartPort, gpio_num_t rxPin) : uartPort(uartPort), rxPin(rxPin), initiliazed(false), eventGroup(xEventGroupCreate()) {}

void GpsNmea::init() {
    if(!initiliazed) {
        initiliazed = true;    
        nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
        config.uart.rx_pin = rxPin;
        config.uart.uart_port = uartPort;
        nmeaHandler = nmea_parser_init(&config);
        addLocationUpdateHandler(gpsUpdateHandler, this);
    }
}

const gps_t& GpsNmea::senseLastKnownLocation() {
    xEventGroupWaitBits(eventGroup, BIT0, pdTRUE, pdTRUE, portMAX_DELAY);
    return lastKnownLocation;
}

void GpsNmea::deinit() {
    if(initiliazed) {
        initiliazed = false;
        removeLocationUpdateHandler(gpsUpdateHandler);
        nmea_parser_deinit(nmeaHandler);
    }
}
//PRIVATE
void GpsNmea::addLocationUpdateHandler(esp_event_handler_t handler, void* eventArg) {
    nmea_parser_add_handler(nmeaHandler, handler, eventArg);
}

void GpsNmea::removeLocationUpdateHandler(esp_event_handler_t handler) {
    nmea_parser_remove_handler(nmeaHandler, handler);
}

void GpsNmea::gpsUpdateHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    GpsNmea* gpsDevice = (GpsNmea*) event_handler_arg;
    switch (event_id)
    {
        case GPS_UPDATE: {
            //printf("GPS: UPDATE RECEIVED\n");
            gps_t* gps = (gps_t*) event_data;
            gpsDevice->lastKnownLocation = *gps; // implict copy
            xEventGroupSetBits(gpsDevice->eventGroup, BIT0);
            break;
        }
        case GPS_UNKNOWN: /* ignored */ break;
    }
}