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
#define DEVICE_NAME "waterlevel:cesena:1";
const int SAMPLING_COUNT = 10;

const Buffer buffer(5);

//extern "C" void app_main(void);

void ground(void) {
    parser::WaterLevelParser* waterLevelParser = new parser::WaterLevelParser();
    parser::LocationParser* locationParser = new parser::LocationParser();
    Parser* parsers[] { new parser::WaterLevelParser(), new parser::LocationParser() };
    task::ParserSet parserSet { parsers, 2 };
    //SETUP POWER MANAGEMENT ! TODO
    //SETUP CONNECTION
    network::Wifi net("DELL", "12345678");
    //middleware::FakeMiddleware middleware;
    middleware::AwsPrivacyConfig privacySetting(
        (const char *)certificate_pem_crt_start,
        (const char *)private_pem_key_start,
        (const char *)aws_root_ca_pem_start
    );
    //printf("%s\n", certificate_pem_crt_start);
    middleware::MqttConfig mqttConfig(
        "a1l0qetj8lwb0i-ats.iot.eu-west-3.amazonaws.com",
         AWS_IOT_MQTT_PORT
    );
    middleware::AwsIotCoreConfig iotConfig { 1 };
    middleware::AwsCoreService middleware(DEVICE_NAME, privacySetting, mqttConfig, iotConfig);
    //SETUP TIMESTAP TODO!
    //SENSORS CREATION
    //support::GpsNmea gps(GPS_SERIAL, GPS_PIN);
    support::DS18B20 ds18b20(DS_PIN, support::P9);
    support::Sonar sonar(TRIG_PIN, ECHO_PIN);
    //TASKs CREATION
    task::WaterLevelTask waterLevelTask(buffer, sonar, ds18b20, SAMPLING_COUNT);
    //task::LocationTask locationTask(buffer, gps);
    task::Consumer consumer(buffer, middleware, net, parserSet);
    task::Task::deployEsp32(waterLevelTask, 30000, 1024, "water_level");
    task::Task::deployEsp32(consumer, 60000, 9012, "consumer");
    //task::Task::deployEsp32(locationTask, 500, 4096, "gps");
    vTaskDelay(portMAX_DELAY);
}