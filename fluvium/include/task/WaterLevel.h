#pragma once
#include "Buffering.h"
#include "Task.h"
#include "Metric.h"
#include "device/DistanceSensor.h"
#include "device/TemperatureSensor.h"


namespace buffering {
    #define waterLevelType 1

    namespace data {
        struct WaterLevel : public Data {
            const metric::meter level;
            WaterLevel(metric::meter level) : Data(waterLevelType), level(level) {}
        };
    }
    namespace parser {
        class WaterLevelParser : public Parser {
            public:
                WaterLevelParser();
            protected:
                json doSerialize(const Data& data) override;
        };
    }
}
namespace task {
    class WaterLevelTask : public Task {
        private:
            const int samplingCount;
            device::DistanceSensor& distanceSensor;
            device::TemperatureSensor& temperatureSensor;
        public:
            WaterLevelTask(const Buffer& buffer, device::DistanceSensor& distSens, device::TemperatureSensor& tempSens, int distanceSampling);
            void run() override;
    };
}