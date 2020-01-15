
#include "Buffering.h"
#include "WaterLevel.h"

#include <freertos/task.h>
#include <esp_task.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

using namespace buffering;
using namespace buffering::data;

extern "C" void app_main(void);

Buffer buffer(2);

static void task_read(void* arg) {
    Buffer* buffer = (Buffer*) arg;
    parser::WaterLevelParser parser;
    auto data = buffer->dequeue();
    printf("Data: %s\n", parser.parse(*data).c_str());
    vTaskDelete(NULL);
}


void app_main(void) {
    WaterLevel* w = new WaterLevel(20);
    buffer.queue(w);
    xTaskCreate(task_read, "task", 2048, &buffer, 5, NULL);
}