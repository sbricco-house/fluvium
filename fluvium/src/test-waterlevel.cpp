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

const gpio_num_t DS_PIN = GPIO_NUM_21;
const gpio_num_t TRIG_PIN = GPIO_NUM_33;
const gpio_num_t ECHO_PIN = GPIO_NUM_32;

//extern "C" void app_main(void);
/*
void app_main(void) {
    //SENSORS CREATION
    support::DS18B20 ds18b20(DS_PIN, support::P9);
    support::Sonar sonar(TRIG_PIN, ECHO_PIN);
    //TASKs CREATION
    ds18b20.init();
    sonar.init();
    for(int i = 0; i <60; i++) {
        auto temp = ds18b20.senseTemperature();
        auto distance = sonar.senseDistance(temp);
        printf("%f;%lf\n", temp, distance);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelay(portMAX_DELAY);
}*/