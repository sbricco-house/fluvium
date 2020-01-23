#pragma once

#include "Device.h"
#include "nmea_parser.h"

namespace device {
    /**
     * Describe a Gps used to retrieve the location of device
     */
    class Gps : public Device {
        public:
            // Sense the location from Gps, and return the last known location. It's a blocking method.
            virtual const gps_t& senseLastKnownLocation() = 0;
            // Deinitiliaze some initiliazed resources.
            virtual void deinit() = 0;
    };
};