#pragma once

#include "Device.h"
#include "Metric.h"
namespace device {
    /**
     * rain gauge is a device measuring rain quantity. 
     * the metric used in this case is millimeter. 
     * the formula that link the millimeter to liter is:
     *  1mm = 1l of rain on an area of 1 mm^2.
     */
    class RainGauge : public Device {
        public:
            /**
             * return the rain quantity sensed by rain gauge.
             */
            virtual metric::millimeter getRainQuantity() = 0;
            /**
             * reset internal rain quantity sensed by the device.
             */
            virtual void reset() = 0;
            /**
             * return the rain qauntity sensed by rain gauge and reset the quantity.
             * this method do this action in atomical way.
             */
            virtual metric::millimeter getAndResetQuantity() = 0;
    };
}