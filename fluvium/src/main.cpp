
//CONFIG_MAIN_TASK_STACK_SIZE TO INCREASE MAIN TASK SIZE
//SYSTEM INCLUDES
#include <stdio.h>
#include <aws_iot_shadow_interface.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "task/Buffering.h"
#include "application/Application.h"
//CONNECTION INCLUDE
#include "middleware/AwsCertificate.h"
#include "middleware/AwsCoreService.h"
#include "network/Gsm.h"
#include "network/Wifi.h"
#include "network/NetworkFactory.h"
using namespace buffering;
using namespace buffering;

//#define DEVICE_NAME "waterlevel:cesena:1"
#define DEVICE_NAME "ground:cesena:1"

const char* APN_GSM = "TM";
const Buffer buffer(5);

extern "C" void app_main(void);

void app_main(void) {
    network::Wifi net = network::Wifi("DELL", "12345678");
    //network::Gsm net = networkfactory::createGsmTTGO(APN_GSM);
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
    launch_ground(buffer, net, middleware);
}