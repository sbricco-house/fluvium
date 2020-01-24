#include "task/Location.h"

using namespace buffering;
using namespace task;
using namespace data;
using namespace parser;

#define GPS_DATA_SIZE 512
#define INNER_JSON_OBJECT 450
//GPS PARSER IMPLEMENTATION
//LocationParser::LocationParser(short decimalPrecision) : Parser(gpsType), decimalPrecision(decimalPrecision) {};
LocationParser::LocationParser(short decimalPrecision) : Parser(gpsType), decimalPrecision(decimalPrecision) {};

json LocationParser::doSerialize(const Data& data) {
    char payload[GPS_DATA_SIZE];
    char innerJson[INNER_JSON_OBJECT];
    auto location = (data::LocationData*) &data;
    sprintf(innerJson, 
            "{\"timestamp\":%llu,\"fix_timestamp\":%llu,\"latitude\":%lf,\"longitude\":%lf,\"altitude\":%lf,\"hdop\":%f}",
            location->timestamp,
            location->fixTimestamp,
            location->latitude,
            location->longitude,
            location->altitude,
            location->hdop);
    
    jsonStruct_t gpsJson {"gps", innerJson, strlen(innerJson) + 1, SHADOW_JSON_OBJECT, NULL };
    aws_iot_shadow_init_json_document(payload, GPS_DATA_SIZE);
    aws_iot_shadow_add_reported(payload, GPS_DATA_SIZE, 1, &gpsJson);
    aws_iot_finalize_json_document(payload, GPS_DATA_SIZE);
    return std::string(payload);
};

//LOCATION TASK IMPLEMENTATION
LocationTask::LocationTask(const Buffer& buffer, device::Gps& gps) : Task(buffer), gps(gps){};

void LocationTask::run() {
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

    printf("Fix time: %llu\nLatitude: %lf\nLongitude: %lf\nAltitude: %lf\nHDop: %lf\n",
        currentBest->fixTimestamp,
        currentBest->latitude,
        currentBest->longitude,
        currentBest->altitude,
        currentBest->hdop);

    buffer.queue(currentBest);
}
LocationData* LocationTask::locationDataFromNmea(gps_t gps) {
    return new LocationData {
        gpsutils::dateTimeToTimestamp(gps.date, gps.tim),
        gps.latitude,
        gps.longitude,
        gps.altitude,
        gps.dop_h
    };
}