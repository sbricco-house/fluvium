#include "task/WaterLevel.h" 

using namespace task;
using namespace buffering;
using namespace data;
using namespace parser;
#define MAX_WATER_LEVEL_DATA 200
#define INNER_OBJECT_SIZE 100
//WATER LEVEL PARSER IMPLEMENTATION
WaterLevelParser::WaterLevelParser() : Parser(waterLevelType) {}

json WaterLevelParser::doSerialize(const Data& data) {
    char payload[MAX_WATER_LEVEL_DATA];
    char innerObject[INNER_OBJECT_SIZE];
    auto waterLevelData = (WaterLevel*) & data;
    sprintf(innerObject, "{\"%s\": %llu, \"%s\": %f}", 
        "timestamp", waterLevelData->timestamp,
        "delta", waterLevelData->level);

    jsonStruct_t waterLevelJson {"water_level", innerObject, strlen(innerObject) + 1, SHADOW_JSON_OBJECT, NULL };
    aws_iot_shadow_init_json_document(payload, MAX_WATER_LEVEL_DATA);
    aws_iot_shadow_add_reported(payload, MAX_WATER_LEVEL_DATA, 1, &waterLevelJson);
    aws_iot_finalize_json_document(payload, MAX_WATER_LEVEL_DATA);
    
    return std::string(payload);
}
//WATER LEVEL TASK IMPLEMENTATION
WaterLevelTask::WaterLevelTask(const Buffer& buffer, device::DistanceSensor& distSens, device::TemperatureSensor& tempSens, int distanceSampling) 
    : Task(buffer), samplingCount(distanceSampling), distanceSensor(distSens), temperatureSensor(tempSens) {
    distanceSensor.init();
    temperatureSensor.init();
}
void WaterLevelTask::run() {
    float distanceSensed = 0;
    float temp = temperatureSensor.senseTemperature();
    for(int i = 0; i < samplingCount; i++) {
        distanceSensed += distanceSensor.senseDistance(temp);
    }
    auto data = new buffering::data::WaterLevel(distanceSensed / samplingCount);
    buffer.queue(data);
}