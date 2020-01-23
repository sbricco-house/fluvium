#include "task/Task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task::Task::runTask(void * arg) {
    TaskContext * context = (TaskContext *) arg;
    auto xLastWakeTime = xTaskGetTickCount();
    context->task.run();
    auto ticks = pdMS_TO_TICKS(context->schedulingPeriod);
    while(1) {
        vTaskDelayUntil(&xLastWakeTime, ticks);
        context->task.run();
    }
}

void task::Task::deployEsp32(Task& task, 
                        millisecond schedulingPeriod,
                        int taskStackSize,
                        char* name) {
    auto context = new TaskContext { task, schedulingPeriod};
    xTaskCreate(Task::runTask, name, taskStackSize, context, STANDARD_PRIORITY, NULL);
}