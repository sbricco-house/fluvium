#pragma once

#include "Buffering.h"
#include "Task.h"
#include "Gps.h"

namespace buffering {
    #define gpsType 2
    namespace data {
        struct LocationData : public Data {
            long fixTimestamp;
            double latitude;
            double longitude;
            double altitude;
            float hdop;
            LocationData(long fixTimestamp, double latitude, double longitude, double altitude, float hdop) :
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
                static const short SERIALIZE_BUFFER = 80;
                short decimalPrecision;
            public:
                LocationParser(short decimalPrecision = 6) : Parser(gpsType) {};
            protected:
                csv doSerialize(const Data& data) {
                    if(data.id != gpsType) return "";
                    char buff[SERIALIZE_BUFFER];
                    data::LocationData* location = (data::LocationData*) &data;
                    sprintf(buff, "%ld;%ld;%.7lf;%.7lf;%.7lf;%.3f",
                                    location->timestap,
                                    location->fixTimestamp,
                                    location->latitude,
                                    location->longitude,
                                    location->altitude,
                                    location->hdop);
                    
                    return csv(buff);
                }
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
            LocationTask(const Buffer& buffer, device::Gps& gps) : 
                Task(buffer),
                gps(gps){};

            void run() override {
                LocationData* currentBest = nullptr;
                // some delay for wait a gps fix
                vTaskDelay(pdMS_TO_TICKS(DELAY_MS_FOR_FIX));
                
                gps.init();
                for(int i = 0; i < TARGET_NUMBER_SAMPLES; ) {   
                    gps_t lastKnownLocation = gps.senseLastKnownLocation();
                    // only valid location are considered valid sample
                    if(lastKnownLocation.valid) {
                        if(currentBest == nullptr) {
                            currentBest = locationDataFromNmea(lastKnownLocation);
                        }
                        else if(lastKnownLocation.dop_h < currentBest->hdop) {
                            delete currentBest;
                            currentBest = locationDataFromNmea(lastKnownLocation);
                        } 
                        i++;
                    }
                }
                gps.deinit();

                printf("Fix time: %ld\nLatitude: %lf\nLongitude: %lf\nAltitude: %lf\nHDop: %lf\n",
                    currentBest->fixTimestamp,
                    currentBest->latitude,
                    currentBest->longitude,
                    currentBest->altitude,
                    currentBest->hdop);

                buffer.queue(currentBest);
            };
        private:
            static LocationData* locationDataFromNmea(gps_t gps) {
                return new LocationData {
                    (long) gpsutils::dateTimeToTimestamp(gps.date, gps.tim),
                    gps.latitude,
                    gps.longitude,
                    gps.altitude,
                    gps.dop_h
                };
            }
    };
}