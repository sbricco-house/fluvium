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
const gpio_num_t DS_PIN = GPIO_NUM_21; //GPIO where you connected ds18b20

extern "C" void app_main();
#include "GPS.h"

using namespace buffering;
using namespace buffering::data;

extern "C" void app_main(void);

const Buffer buffer(10);

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

void app_main(void) {
    device::SerialNmeaGps gps(UART_NUM_2, GPIO_NUM_16);
    task::LocationTask task(buffer, gps);
    xTaskCreate(task_read, "task-read", 2048, (void*)&buffer, 5, NULL);
    task.run();
}