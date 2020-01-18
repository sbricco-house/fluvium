#pragma once 
#include "Device.h"
#include "Metric.h"
namespace device {
    class TemperatureSensor : public Device {
        public:
            virtual metric::celsius senseTemperature();
    }; 
}