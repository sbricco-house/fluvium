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

const char* APN_SEND_SPEED = "TM";
const gpio_num_t DS_PIN = GPIO_NUM_21;
const uart_port_t GPS_SERIAL = UART_NUM_2;
const gpio_num_t GPS_PIN = GPIO_NUM_12;
const gpio_num_t TRIG_PIN = GPIO_NUM_33;
const gpio_num_t ECHO_PIN = GPIO_NUM_32;
const metric::millimeter DELTA_QUANTITY = 0.08;
const int SAMPLING_COUNT = 10;

const Buffer buffer(5);

extern "C" void app_main(void);

void app_main(void) {
    boot::countBoot();
    parser::WaterLevelParser* waterLevelParser = new parser::WaterLevelParser();
    //SETUP CONNECTION
    network::Gsm net = networkfactory::createGsmTTGO(APN_SEND_SPEED);
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
    support::DS18B20 ds18b20(DS_PIN, support::P9);
    support::Sonar sonar(TRIG_PIN, ECHO_PIN);

    net.connect();
    middleware.connect();
    ds18b20.init();
    sonar.init();
    for(int i = 0; i < 10; i++) {
        auto temp = ds18b20.senseTemperature();
        auto distance = sonar.senseDistance(temp);
        auto data = data::WaterLevel(distance);
        printf("%llu", data.timestamp);
        auto json = waterLevelParser->serialize(data);
        middleware.publish((char *)json.c_str());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    //TASKs CREATION
    vTaskDelay(portMAX_DELAY);
}