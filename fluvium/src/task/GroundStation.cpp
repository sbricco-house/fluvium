#include "task/GroundStation.h"
#include "Metric.h"
#include <string.h>
using namespace buffering;
using namespace task;
using namespace data;
using namespace parser;
//GROUND DATA PARSER IMPLEMENTATION
#define MAX_GROUND_DATA 200
#define INNER_JSON_OBJECT 150
GroundDataParser::GroundDataParser() : Parser(groundType) {}

json GroundDataParser::doSerialize(const Data& data) {
    char payload[MAX_GROUND_DATA]; 
    char innerJson[INNER_JSON_OBJECT];
    auto groundData = (GroundData*) & data;

    sprintf(innerJson, "{\"%s\":%llu, \"%s\": %f, \"%s\":%lf}",
        "timestamp", groundData->timestamp,
        "soil_moisture", groundData->soilMoisture,
        "rain_quantity", groundData->rainQuantity);
    
    jsonStruct_t groundDataJson {"ground", innerJson, strlen(innerJson) + 1, SHADOW_JSON_OBJECT, NULL };
    aws_iot_shadow_init_json_document(payload, MAX_GROUND_DATA);
    aws_iot_shadow_add_reported(payload, MAX_GROUND_DATA, 1, &groundDataJson);
    aws_iot_finalize_json_document(payload, MAX_GROUND_DATA);

    return std::string(payload);
}
//GROUND TASK IMPLEMENTATION
GroundStationTask::GroundStationTask(const Buffer& buffer,device::SoilMoistureSensor& soilMoistureSensor,device::RainGauge& rainGuageSensor,int soilMoistureSamplingCount)
    : Task(buffer), soilMoistureSensor(soilMoistureSensor), rainGaugeSensor(rainGuageSensor), soilMoistureSamplingCount(soilMoistureSamplingCount) {    
    soilMoistureSensor.init();
    rainGaugeSensor.init();
}

void GroundStationTask::run() {
    auto avgSoilMoisture = senseAvgSoilMoisture();
    auto data = new data::GroundData {
        avgSoilMoisture,
        rainGaugeSensor.getAndResetQuantity()
    };

    buffer.queue(data); 
}

metric::relativehumidity GroundStationTask::senseAvgSoilMoisture() {
    metric::relativehumidity avgHumidity = 0;
    for(int i = 0; i < soilMoistureSamplingCount; i++) {
        avgHumidity += soilMoistureSensor.senseSoilMoisture();
    }
    return avgHumidity / soilMoistureSamplingCount;
}
