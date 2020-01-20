#pragma once

#include "Buffering.h"
#include "Task.h"
#include "Metric.h"
#include "SoilMoistureSensor.h"
#include "RainGauge.h"
#include <cstdio>

// TODO: change Station2Data name
namespace buffering {
    #define station2Type 4

    namespace data {
        struct GroundData : public Data {
            const metric::relativehumidity soilMoisture;
            const metric::millimeter rainQuantity; // TODO: replace type with metric
            GroundData(metric::relativehumidity soilMoisture, metric::millimeter rainQuantity) : 
                Data(station2Type), soilMoisture(soilMoisture), rainQuantity(rainQuantity) {};
        };
    }
    namespace parser {
        class GroundDataParser : public Parser {
            #define parser_buffer_size 20
            public:
                GroundDataParser() : Parser(station2Type) {}
            protected: 
                csv doSerialize(const Data& data) override {
                    data::GroundData* stationData  = (data::GroundData*) &data;
                    char buffer[parser_buffer_size];
                    sprintf(buffer, "%ld;%.4lf;%.4lf", 
                        stationData->timestap, 
                        stationData->soilMoisture, 
                        stationData->rainQuantity);
                    return csv(buffer);
                }
        };
    }
}
namespace task {
    class GroundStationTask : public Task {
        private:
            device::SoilMoistureSensor& soilMoistureSensor;
            device::RainGauge& rainGaugeSensor;
            int soilMoistureSamplingCount;
        public:
            GroundStationTask(
                        const Buffer& buffer,
                        device::SoilMoistureSensor& soilMoistureSensor,
                        device::RainGauge& rainGuageSensor,
                        int soilMoistureSamplingCount) :
                Task(buffer), 
                soilMoistureSensor(soilMoistureSensor),
                rainGaugeSensor(rainGuageSensor),
                soilMoistureSamplingCount(soilMoistureSamplingCount) {    
                    soilMoistureSensor.init();
                }
            
            void run() override {
                auto avgSoilMoisture = senseAvgSoilMoisture();
                auto data = new data::GroundData {
                    avgSoilMoisture,
                    rainGaugeSensor.getAndResetQuantity()
                };

                buffer.queue(data);                
            }
        
        private:
            metric::relativehumidity senseAvgSoilMoisture() {
                metric::relativehumidity avgHumidity = 0;
                for(int i = 0; i < soilMoistureSamplingCount; i++) {
                    avgHumidity += soilMoistureSensor.senseSoilMoisture();
                }
                return avgHumidity / soilMoistureSamplingCount;
            }
    };
}