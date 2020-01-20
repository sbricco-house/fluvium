#pragma once

#include "Buffering.h"
#include "Task.h"
#include "Metric.h"
#include "SoilMoistureSensor.h"
#include <cstdio>

// TODO: change Station2Data name
namespace buffering {
    #define station2Type 4

    namespace data {
        struct Station2Data : public Data {
            const metric::relativehumidity soilMoisture;
            const float rainQuantity; // TODO: replace type with metric
            Station2Data(metric::relativehumidity soilMoisture, float rainQuantity) : 
                Data(station2Type), soilMoisture(soilMoisture), rainQuantity(rainQuantity) {};
        };
    }
    namespace parser {
        class Station2Parser : public Parser {
            #define parser_buffer_size 20
            public:
                Station2Parser() : Parser(station2Type) {}
            protected: 
                csv doSerialize(const Data& data) override {
                    if(data.id != station2Type) return "";
                    data::Station2Data* stationData  = (data::Station2Data*) &data;
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
    class Station2Task : public Task {
        private:
            device::SoilMoistureSensor& soilMoistureSensor;
            int soilMoistureSamplingCount;
            // TODO: add rain gauge sensor
            metric::relativehumidity senseAvgSoilMoisture();
        public:
            Station2Task(
                        const Buffer& buffer,
                        device::SoilMoistureSensor& soilMoistureSensor,
                        int soilMoistureSamplingCount) :
                Task(buffer), 
                soilMoistureSensor(soilMoistureSensor),
                soilMoistureSamplingCount(soilMoistureSamplingCount) {    
                    soilMoistureSensor.init();
                }
            
            void run() override {
                auto avgSoilMoisture = senseAvgSoilMoisture();
                printf("LOG Station2: %.4lf\n", avgSoilMoisture);

                auto data = new data::Station2Data {
                    avgSoilMoisture,
                    0 // TODO: replace with rain gauge value
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