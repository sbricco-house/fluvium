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

using namespace buffering;
using namespace buffering::data;

#define DEVICE_NAME "waterlevel:cesena:1"

const int PERIOD_CONSUMER = 8 * 60000; // 8 min
const int PERIOD_WATER_LEVEL = 5 * 60000; // 5 min
const int PERIOD_GPS = 10 * 60000; // 10 min

const char* APN_GROUND = "TM";
const gpio_num_t DS_PIN = GPIO_NUM_21;
const uart_port_t GPS_SERIAL = UART_NUM_2;
const gpio_num_t GPS_PIN = GPIO_NUM_12;
const gpio_num_t TRIG_PIN = GPIO_NUM_33;
const gpio_num_t ECHO_PIN = GPIO_NUM_32;
const metric::millimeter DELTA_QUANTITY = 0.08;
const int SAMPLING_COUNT = 10;

const Buffer buffer(5);

//extern "C" void app_main(void);

/*void app_main(void) {
    boot::countBoot();
    parser::WaterLevelParser* waterLevelParser = new parser::WaterLevelParser();
    parser::LocationParser* locationParser = new parser::LocationParser();
    Parser* parsers[] { waterLevelParser, locationParser };
    task::ParserSet parserSet { parsers, 2 };
    //SETUP CONNECTION
    network::Gsm net = networkfactory::createGsmTTGO(APN_GROUND);
    //SETUP TIME AT FIRST BOOT
    boot::setupTimeAtFirstBoot(net);
    middleware::AwsPrivacyConfig privacySetting(
        (const char *)certificate_pem_crt_start,
        (const char *)private_pem_key_start,
        (const char *)aws_root_ca_pem_start
    );
    middleware::MqttConfig mqttConfig(
        "a1l0qetj8lwb0i-ats.iot.eu-west-2.amazonaws.com",
         AWS_IOT_MQTT_PORT
    );
    middleware::AwsIotCoreConfig iotConfig { 1 };
    middleware::AwsCoreService middleware(DEVICE_NAME, privacySetting, mqttConfig, iotConfig);
    //SETUP TIMESTAP TODO!
    //SENSORS CREATION
    support::GpsNmea gps(GPS_SERIAL, GPS_PIN);
    support::DS18B20 ds18b20(DS_PIN, support::P9);
    support::Sonar sonar(TRIG_PIN, ECHO_PIN);
    //TASKs CREATION
    task::WaterLevelTask waterLevelTask(buffer, sonar, ds18b20, SAMPLING_COUNT);
    task::LocationTask locationTask(buffer, gps);
    task::Consumer consumer(buffer, middleware, net, parserSet);
    //TASKs START
    task::Task::deployEsp32(waterLevelTask, PERIOD_WATER_LEVEL, 1024, "water_level");
    task::Task::deployEsp32(consumer, PERIOD_CONSUMER, 9012, "consumer");
    task::Task::deployEsp32(locationTask, PERIOD_GPS, 4096, "gps");
    vTaskDelay(portMAX_DELAY);
}*/