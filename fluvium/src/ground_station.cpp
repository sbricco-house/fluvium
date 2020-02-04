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

#define DEVICE_NAME "ground:cesena:1"

const int PERIOD_CONSUMER = 8 * 60000; // 8 min
const int PERIOD_GROUND = 20 * 60000; // 20 min

const char* APN = "TM";
const gpio_num_t REED_SWITCH_PIN = GPIO_NUM_12;
const gpio_num_t SOIL_PIN = GPIO_NUM_2;
const adc1_channel_t SOIL_INPUT_PIN = ADC1_CHANNEL_5;
const adc_bits_width_t SOIL_PRECISION = ADC_WIDTH_BIT_9;
const metric::millimeter DELTA_QUANTITY = 0.08;
const int SAMPLING_COUNT = 10;

const Buffer buffer(5);

extern "C" void app_main(void);

void app_main(void) {
    boot::countBoot();
    parser::GroundDataParser* groundDataParser = new parser::GroundDataParser();
    Parser* parsers[] { groundDataParser };
    task::ParserSet parserSet { parsers, 1 };
    //SETUP CONNECTION
    network::Gsm net = networkfactory::createGsmTTGO(APN);
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
    support::PulseRain rainGauge(REED_SWITCH_PIN, DELTA_QUANTITY);
    support::SparkFunMoisture soilMoisture(SOIL_PIN, SOIL_INPUT_PIN, SOIL_PRECISION);
    //TASKs CREATION
    task::GroundStationTask groundStationTask(buffer, soilMoisture, rainGauge, SAMPLING_COUNT);
    task::Consumer consumer(buffer, middleware, net, parserSet);
    //TASKs START
    task::Task::deployEsp32(groundStationTask, PERIOD_GROUND, 2024, "ground_station");
    task::Task::deployEsp32(consumer, PERIOD_CONSUMER, 9012, "consumer");
    vTaskDelay(portMAX_DELAY);
}