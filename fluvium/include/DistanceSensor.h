#pragma once
#include "Device.h"
#include "Metric.h"
namespace device
{
    /**
     * describe a sensor used to measure distance from a fixed point.
     */
    class DistanceSensor : public Device {
        public:
            virtual metric::meter senseDistance(metric::celsius temperature);
    };
}
