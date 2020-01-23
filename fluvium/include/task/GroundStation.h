#pragma once

#include "Buffering.h"
#include "Task.h"
#include "Metric.h"
#include "device/SoilMoistureSensor.h"
#include "device/RainGauge.h"

namespace buffering {
    #define groundType 4

    namespace data {
        struct GroundData : public Data {
            const metric::relativehumidity soilMoisture;
            const metric::millimeter rainQuantity; // TODO: replace type with metric
            GroundData(metric::relativehumidity soilMoisture, metric::millimeter rainQuantity) : 
                Data(groundType), soilMoisture(soilMoisture), rainQuantity(rainQuantity) {};
        };
    }

    namespace parser {
        class GroundDataParser : public Parser {
            #define parser_buffer_size 20
            public:
                GroundDataParser();
            protected: 
                json doSerialize(const Data& data) override;
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
                        int soilMoistureSamplingCount);
            
            void run() override;
        
        private:
            metric::relativehumidity senseAvgSoilMoisture();
    };
}