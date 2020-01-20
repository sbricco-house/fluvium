#pragma once

#include "Device.h"
#include "Metric.h"

namespace device {
    class SoilMoistureSensor : public Device {
        public:
            virtual metric::relativehumidity senseSoilMoisture() = 0;
    };
}