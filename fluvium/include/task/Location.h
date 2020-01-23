#pragma once

#include "Buffering.h"
#include "Task.h"
#include "device/Gps.h"

namespace buffering {
    #define gpsType 2
    namespace data {
        struct LocationData : public Data {
            long long unsigned fixTimestamp;
            double latitude;
            double longitude;
            double altitude;
            float hdop;
            LocationData(long long unsigned fixTimestamp, double latitude, double longitude, double altitude, float hdop) :
                Data(gpsType),
                fixTimestamp(fixTimestamp),
                latitude(latitude),
                longitude(longitude),
                altitude(altitude),
                hdop(hdop) {};
        };
    }

    namespace parser {
        class LocationParser : public Parser {
            private:
                short decimalPrecision;
            public:
                LocationParser(short decimalPrecision = 6);
            protected:
                json doSerialize(const Data& data) override;
        };
    }
}

// TODO: consider only valid gps position could generate an
// infinite waiting time, consider to add timeout
// TODO: add timestamp to location provided by satellites
namespace task {
    using namespace buffering::data;
    
    class LocationTask : public Task {
        private:
            static const int TARGET_NUMBER_SAMPLES = 10;
            static const short DELAY_MS_FOR_FIX = 10000; // 10 seconds 
            device::Gps& gps;         

        public:
            LocationTask(const Buffer& buffer, device::Gps& gps);

            void run() override;
        private:
            static LocationData* locationDataFromNmea(gps_t gps);
    };
}