#pragma once

#include "Buffering.h"

namespace task {
    using namespace buffering;
    class Task {
        protected:
            const Buffer& buffer;
        public:
            Task(const Buffer& buffer) : buffer(buffer) {};
            virtual void run();
    };
}