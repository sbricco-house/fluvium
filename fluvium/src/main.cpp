#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "DS18B20Support.h"
#include "SonarSupport.h"
#include "WaterLevel.h"
#include "PulseRainSupport.h"
#include "TemperatureSensor.h"
#include "Location.h"
#include "GpsNmeaSupport.h"
#include "GroundStation.h"
#include "SoilMoistureSupport.h"

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

const Buffer buffer(10);

extern "C" void app_main(void);

//TODO create task that consume and send using gsm.
static void task_read(void* arg) {
    Buffer* buffer = (Buffer*) arg;
    parser::LocationParser parser;
    while(1) {
        auto data = buffer->dequeue();
        printf("Data: %s\n", parser.serialize(*data).c_str());
    }
    vTaskDelete(NULL);
}

void app_main(void) {
    //SETUP POWER MANAGEMENT
    //SETUP TIMESTAP TODO!
    //SENSORS CREATION
    support::GpsNmea gps(GPS_SERIAL, GPS_PIN);
    support::DS18B20 ds18b20(GPIO_NUM_5, support::P9);
    support::Sonar sonar(TRIG_PIN, ECHO_PIN);
    support::PulseRain rainGauge(REED_SWITCH_PIN, DELTA_QUANTITY);
    support::SparkFunMoisture soilMoisture(SOIL_PIN, SOIL_INPUT_PIN, SOIL_PRECISION);
    //TASKs CREATION
    task::WaterLevelTask waterLevelTask(buffer, sonar, ds18b20, SAMPLING_COUNT);
    task::LocationTask locationTask(buffer, gps);
    task::GroundStationTask groundStationTask(buffer, soilMoisture, rainGauge, SAMPLING_COUNT);
    task::Task::deployEsp32(waterLevelTask, 5000, 768, "water_level");
    task::Task::deployEsp32(groundStationTask, 5000, 768, "groud_station");
    xTaskCreate(task_read, "task-read", 2048, (void*)&buffer, 5, NULL);
    vTaskDelay(portMAX_DELAY);
}