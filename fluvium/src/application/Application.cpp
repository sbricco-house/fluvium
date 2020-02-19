#include "application/Application.h"
//CONFIG_MAIN_TASK_STACK_SIZE TO INCREASE MAIN TASK SIZE
//SYSTEM INCLUDES
#include <stdio.h>
#include <aws_iot_shadow_interface.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
//SUPPORT INCLUDE
#include "support/DS18B20Support.h"
#include "support/SonarSupport.h"
#include "support/PulseRainSupport.h"
#include "support/GpsNmeaSupport.h"
#include "support/SoilMoistureSupport.h"
//CONNECTION INCLUDE
#include "network/FakeNetwork.h"
#include "middleware/FakeMiddleware.h"
#include "middleware/AwsCertificate.h"
#include "middleware/AwsCoreService.h"
#include "network/Wifi.h"
#include "network/NetworkFactory.h"
//TASK INCLUDE
#include "task/WaterLevel.h"
#include "task/Location.h"
#include "task/GroundStation.h"
#include "task/Consumer.h"
//SUPPORT UTILITIES
#include "Boot.h"
//COMMON
const int PERIOD_CONSUMER = 1 * 60000; //  min
const int SAMPLING_COUNT = 10;
//real value gps = 10 MIN; consumer 8 MIN; waterlevel 5 MIN;
const int PERIOD_WATER_LEVEL = 1 * 60000; // 1 min
const int PERIOD_GPS = 1 * 60000; // 1 min
const gpio_num_t DS_PIN = GPIO_NUM_21;
const uart_port_t GPS_SERIAL = UART_NUM_2;
const gpio_num_t GPS_PIN = GPIO_NUM_12;
const gpio_num_t TRIG_PIN = GPIO_NUM_33;
const gpio_num_t ECHO_PIN = GPIO_NUM_32;

void launch_waterlevel(const Buffer & buffer, network::Network& net, middleware::MiddlewareService& service){
    boot::countBoot();
    parser::WaterLevelParser* waterLevelParser = new parser::WaterLevelParser();
    parser::LocationParser* locationParser = new parser::LocationParser();
    Parser* parsers[] { waterLevelParser, locationParser };
    task::ParserSet parserSet { parsers, 2 };
    //SENSORS CREATION
    support::GpsNmea gps(GPS_SERIAL, GPS_PIN);
    support::DS18B20 ds18b20(DS_PIN, support::P9);
    support::Sonar sonar(TRIG_PIN, ECHO_PIN);
    //SETUP TIMESTAMP
    boot::setupTimeAtFirstBoot(net);
    //TASKs CREATION
    task::WaterLevelTask waterLevelTask(buffer, sonar, ds18b20, SAMPLING_COUNT);
    task::LocationTask locationTask(buffer, gps);
    task::Consumer consumer(buffer, service, net, parserSet);
    //TASKs START
    task::Task::deployEsp32(waterLevelTask, PERIOD_WATER_LEVEL, 1024, "water_level");
    task::Task::deployEsp32(consumer, PERIOD_CONSUMER, 9012, "consumer");
    task::Task::deployEsp32(locationTask, PERIOD_GPS, 4096, "gps");
    vTaskDelay(portMAX_DELAY);
}

const int PERIOD_GROUND = 1000 * 60; // 5 min
const gpio_num_t REED_SWITCH_PIN = GPIO_NUM_18;
const gpio_num_t SOIL_PIN = GPIO_NUM_2;
const adc1_channel_t SOIL_INPUT_PIN = ADC1_CHANNEL_5;
const adc_bits_width_t SOIL_PRECISION = ADC_WIDTH_BIT_9;
const metric::millimeter DELTA_QUANTITY = 0.08;

void launch_ground(const Buffer & buffer, network::Network& net, middleware::MiddlewareService& service){
    boot::countBoot();
    parser::GroundDataParser* groundDataParser = new parser::GroundDataParser();
    Parser* parsers[] { groundDataParser };
    task::ParserSet parserSet { parsers, 1 };
    //SUPPORT CREATION
    support::PulseRain rainGauge(REED_SWITCH_PIN, DELTA_QUANTITY);
    support::SparkFunMoisture soilMoisture(SOIL_PIN, SOIL_INPUT_PIN, SOIL_PRECISION);
    //TASKs CREATION
    task::GroundStationTask groundStationTask(buffer, soilMoisture, rainGauge, SAMPLING_COUNT);
    task::Consumer consumer(buffer, service, net, parserSet);
    //SETUP TIMESTAMP
    boot::setupTimeAtFirstBoot(net);
    //TASKs START
    task::Task::deployEsp32(groundStationTask, PERIOD_GROUND, 2024, "ground_station");
    task::Task::deployEsp32(consumer, PERIOD_CONSUMER, 9012, "consumer");
    vTaskDelay(portMAX_DELAY);
}