#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "DS18B20Support.h"
#include "SonarSupport.h"
#include "WaterLevel.h"
#include "TemperatureSensor.h"
#include "Location.h"
#include "GpsNmeaSupport.h"
using namespace buffering;
using namespace buffering::data;

const gpio_num_t DS_PIN = GPIO_NUM_21; //GPIO where you connected ds18b20
const uart_port_t GPS_SERIAL = UART_NUM_2;
const gpio_num_t GPS_PIN = GPIO_NUM_16;

const Buffer buffer(10);

extern "C" void app_main(void);

// Example task for read from buffer and serialize the location data
static void task_read(void* arg) {
    Buffer* buffer = (Buffer*) arg;
    parser::LocationParser parser;
    while(1) {
        auto data = buffer->dequeue();
        printf("Data: %s\n", parser.serialize(*data).c_str());
    }
    vTaskDelete(NULL);
}

/*
void app_main(void) {
    support::GpsNmea gps(GPS_SERIAL, GPS_PIN);
    task::LocationTask task(buffer, gps);
    xTaskCreate(task_read, "task-read", 2048, (void*)&buffer, 5, NULL);
    task.run();
} */