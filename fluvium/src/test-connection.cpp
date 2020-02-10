
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

const int PERIOD_CONSUMER = 8 * 1000; // 8 min
const int PERIOD_GROUND = 20 * 1000; // 20 min

const char* APN_TTGO = "TM";
const gpio_num_t REED_SWITCH_PIN = GPIO_NUM_23;
const gpio_num_t SOIL_PIN = GPIO_NUM_2;
const adc1_channel_t SOIL_INPUT_PIN = ADC1_CHANNEL_5;
const adc_bits_width_t SOIL_PRECISION = ADC_WIDTH_BIT_9;
const metric::millimeter DELTA_QUANTITY = 0.08;
const int SAMPLING_COUNT = 10;

const Buffer buffer(5);

//extern "C" void app_main(void);

void app_main(void) {
    //TEST CONNECTION SPPED
    network::Gsm net = networkfactory::createGsmTTGO(APN_TTGO);
    //network::Wifi net = network::Wifi("DELL", "12345678");
    auto start = timeutils::timestampMs();
    net.connectWithAttempts(5);
    auto delta = timeutils::timestampMs() - start;
    printf("Delta: %llu\n", delta);
    net.disconnect();
    start = timeutils::timestampMs();
    net.connectWithAttempts(5);
    delta = timeutils::timestampMs() - start;
    printf("Second Delta: %llu\n", delta);
    
}