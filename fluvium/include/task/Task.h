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
            static void runTask(void * arg);
        protected:
            const Buffer& buffer;
        public:
            Task(const Buffer& buffer) : buffer(buffer) {};
            virtual void run() = 0;
            static void deployEsp32(Task& task, millisecond schedulingPeriod, int taskStackSize, char* name);
    };
}