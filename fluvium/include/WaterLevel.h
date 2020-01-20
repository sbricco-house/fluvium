#pragma once
#include "Buffering.h"
#include "Task.h"
#include "Metric.h"
#include "DistanceSensor.h"
#include "TemperatureSensor.h"
#include <cstdio>
#include <driver/gpio.h>
namespace buffering {
    #define waterLevelType 1

    namespace data {
        struct WaterLevel : public Data {
            metric::meter level;
            WaterLevel(metric::meter level) : Data(waterLevelType) {
                this->level = level;
            }
        };
    }
    namespace parser {
        class WaterLevelParser : public Parser {
            #define parser_buffer_size 20 // <uint_16.charlength>;<long.charlength>;
            public:
                WaterLevelParser() : Parser(waterLevelType) {}
            protected: 
                csv doSerialize(const Data& data) override {
                    data::WaterLevel& level = *(data::WaterLevel*)(&data);
                    char buffer[parser_buffer_size];
                    sprintf(buffer, "%.4lf;%ld", level.level, level.timestap);
                    return csv(buffer);
                }
        };
    }
}
namespace task {
    class WaterLevelTask : public Task {
        private:
            int samplingCount;
            device::DistanceSensor& distanceSensor;
            device::TemperatureSensor& temperatureSensor;
        public:
            WaterLevelTask(
                        const Buffer& buffer,
                        device::DistanceSensor& distSens, 
                        device::TemperatureSensor& tempSens, 
                        int distanceSampling) :
                Task(buffer), 
                samplingCount(distanceSampling),
                distanceSensor(distSens),
                temperatureSensor(tempSens) {    
                    distanceSensor.init();
                    temperatureSensor.init();
                }
            
            void run() override {
                float distanceSensed = 0;
                float temp = temperatureSensor.senseTemperature();
                for(int i = 0; i < samplingCount; i++) {
                    distanceSensed += distanceSensor.senseDistance(temp);
                }
                auto data = new buffering::data::WaterLevel(distanceSensed / samplingCount);
                buffer.queue(data);
            }
    };
}