
#include "Buffering.h"
#include "WaterLevel.h"

#include <freertos/task.h>
#include <esp_task.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "GPS.h"

using namespace buffering;
using namespace buffering::data;

extern "C" void app_main(void);

const Buffer buffer(2);

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