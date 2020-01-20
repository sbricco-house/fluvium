#pragma once
#include "Util.h"
#include "Buffering.h"

using namespace timeutils;
namespace task {
    using namespace buffering;
    #define STANDARD_PRIORITY 10
    
    class Task {
        private:
            struct TaskContext {
                task::Task& task;
                millisecond schedulingPeriod;
            };
            static void runTask(void * arg) {
                TaskContext * context = (TaskContext *) arg;
                auto xLastWakeTime = xTaskGetTickCount();
                context->task.run();
                auto ticks = pdMS_TO_TICKS(context->schedulingPeriod);
                while(1) {
                    vTaskDelayUntil(&xLastWakeTime, ticks);
                    context->task.run();
                }
            }
        protected:
            const Buffer& buffer;
        public:
            Task(const Buffer& buffer) : buffer(buffer) {};
            virtual void run();
            static void deployEsp32(Task& task, 
                                    millisecond schedulingPeriod,
                                    int taskStackSize,
                                    char* name) {
                auto context = new TaskContext { task, schedulingPeriod};
                xTaskCreate(Task::runTask, name, taskStackSize, context, STANDARD_PRIORITY, NULL);
            }
    };
}