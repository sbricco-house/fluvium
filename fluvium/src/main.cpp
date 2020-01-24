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
#include "network/Wifi.h"
#include "middleware/AwsCoreService.h"
#include "middleware/AwsCertificate.h"
//TASK INCLUDE
#include "task/WaterLevel.h"
#include "task/Location.h"
#include "task/GroundStation.h"
#include "task/Consumer.h"

using namespace buffering;
using namespace buffering::data;

const gpio_num_t DS_PIN = GPIO_NUM_21; //GPIO where you connected ds18b20
const uart_port_t GPS_SERIAL = UART_NUM_2;
const gpio_num_t GPS_PIN = GPIO_NUM_16;
const gpio_num_t TRIG_PIN = GPIO_NUM_33;
const gpio_num_t ECHO_PIN = GPIO_NUM_35;
const gpio_num_t REED_SWITCH_PIN = GPIO_NUM_12;
const gpio_num_t SOIL_PIN = GPIO_NUM_2;
const adc1_channel_t SOIL_INPUT_PIN = ADC1_CHANNEL_0;
const adc_bits_width_t SOIL_PRECISION = ADC_WIDTH_BIT_9;
const metric::millimeter DELTA_QUANTITY = 0.08;
const int SAMPLING_COUNT = 10;

const Buffer buffer(100);

extern "C" void app_main(void);

void app_main(void) {
    parser::WaterLevelParser* waterLevelParser = new parser::WaterLevelParser();
    parser::GroundDataParser* groundDataParser = new parser::GroundDataParser();
    parser::LocationParser* locationParser = new parser::LocationParser();
    Parser* parsers[] { waterLevelParser, groundDataParser, locationParser };
    task::ParserSet parserSet { parsers, 3 };
    //SETUP POWER MANAGEMENT ! TODO
    //SETUP CONNECTION
    //network::FakeNetwork net;
    //middleware::FakeMiddleware middleware;
    network::Wifi net("DELL", "12345678");
    middleware::AwsPrivacyConfig privacySetting(
        (const char *)certificate_pem_crt_start,
        (const char *)private_pem_key_start,
        (const char *)aws_root_ca_pem_start
    );
    middleware::MqttConfig mqttConfig(
        "a1l0qetj8lwb0i-ats.iot.us-east-2.amazonaws.com",
         AWS_IOT_MQTT_PORT
    );
    middleware::AwsIotCoreConfig iotConfig { 1 };
    middleware::AwsCoreService middleware("waterlevel:cesena:1", privacySetting, mqttConfig, iotConfig);
    //SETUP TIMESTAP TODO!
    //SENSORS CREATION
    //support::GpsNmea gps(GPS_SERIAL, GPS_PIN);
    support::DS18B20 ds18b20(GPIO_NUM_5, support::P9);
    support::Sonar sonar(TRIG_PIN, ECHO_PIN);
    support::PulseRain rainGauge(REED_SWITCH_PIN, DELTA_QUANTITY);
    support::SparkFunMoisture soilMoisture(SOIL_PIN, SOIL_INPUT_PIN, SOIL_PRECISION);
    //TASKs CREATION
    task::WaterLevelTask waterLevelTask(buffer, sonar, ds18b20, SAMPLING_COUNT);
    //task::LocationTask locationTask(buffer, gps);
    task::Consumer consumer(buffer, middleware, net, parserSet);
    task::GroundStationTask groundStationTask(buffer, soilMoisture, rainGauge, SAMPLING_COUNT);
    task::Task::deployEsp32(waterLevelTask, 1000, 1024, "water_level");
    //task::Task::deployEsp32(groundStationTask, 1000, 2024, "ground_station");
    task::Task::deployEsp32(consumer, 5000, 9012, "consumer");
    //task::Task::deployEsp32(locationTask, 500, 4096, "gps");
    vTaskDelay(portMAX_DELAY);
}