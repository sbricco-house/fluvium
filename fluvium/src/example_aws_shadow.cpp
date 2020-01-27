/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * Additions Copyright 2016 Espressif Systems (Shanghai) PTE LTD
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
/**
 * @file thing_shadow_sample.c
 * @brief A simple connected window example demonstrating the use of Thing Shadow
 *
 * See example README for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "network/Wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "middleware/AwsCoreService.h"
#include "aws_iot_config.h"
#include "middleware/AwsCertificate.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
//TO DEFINED CONFIG_AWS_IOT_MQTT_HOST 
//TO DEFINED AWS_IOT_MQTT_PORT


network::Wifi * wifi;
static const char *TAG = "shadow";

#define ROOMTEMPERATURE_UPPERLIMIT 32.0f
#define ROOMTEMPERATURE_LOWERLIMIT 25.0f
#define STARTING_ROOMTEMPERATURE ROOMTEMPERATURE_LOWERLIMIT

#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID "DELL"
#define EXAMPLE_WIFI_PASS "12345678"

//extern "C" void app_main(void);
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;



static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
    
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        
        ESP_LOGI(TAG, "id = ..%d", event->event_id);
        break;
    }
    return ESP_OK;
}

static bool shadowUpdateInProgress;

void ShadowUpdateStatusCallback(const char *pThingName, ShadowActions_t action, Shadow_Ack_Status_t status,
                                const char *pReceivedJsonDocument, void *pContextData) {
    IOT_UNUSED(pThingName);
    IOT_UNUSED(action);
    IOT_UNUSED(pReceivedJsonDocument);
    IOT_UNUSED(pContextData);

    shadowUpdateInProgress = false;

    ESP_LOGI(TAG, "Entered..");
    if(SHADOW_ACK_TIMEOUT == status) {
        ESP_LOGE(TAG, "Update timed out");
    } else if(SHADOW_ACK_REJECTED == status) {
        ESP_LOGE(TAG, "Update rejected");
    } else if(SHADOW_ACK_ACCEPTED == status) {
        ESP_LOGI(TAG, "Update accepted");
    }
}
void aws_iot_task(void *param) {
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    IoT_Error_t rc = FAILURE;
    middleware::AwsPrivacyConfig privacySetting(
        (const char *)certificate_pem_crt_start,
        (const char *)private_pem_key_start,
        (const char *)aws_root_ca_pem_start
    );
    //printf("%s\n", certificate_pem_crt_start);
    middleware::MqttConfig mqttConfig(
        "a1l0qetj8lwb0i-ats.iot.us-east-2.amazonaws.com",
         AWS_IOT_MQTT_PORT
    );
    middleware::AwsIotCoreConfig iotConfig { 1 };
    middleware::AwsCoreService service("waterlevel:cesena:1", privacySetting, mqttConfig, iotConfig);
    
/*
    // initialize the mqtt client
    AWS_IoT_Client mqttClient;

    ShadowInitParameters_t sp = ShadowInitParametersDefault;
    sp.pHost = "a1l0qetj8lwb0i-ats.iot.us-east-2.amazonaws.com";
    sp.port = AWS_IOT_MQTT_PORT;
    sp.pClientCRT = (const char *)certificate_pem_crt_start;
    sp.pClientKey = (const char *)private_pem_key_start;
    sp.pRootCA = (const char *)aws_root_ca_pem_start;
    
    sp.enableAutoReconnect = false;
    sp.disconnectHandler = NULL;
*/
    /* Wait for WiFI to show as connected */
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    
    ESP_LOGI(TAG, "Shadow Init");
    auto errorConnection = service.connect();
    if(errorConnection != middleware::OK) {
        printf("error..");
        abort();
    }
    /*rc = aws_iot_shadow_init(&mqttClient, &sp);
    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "aws_iot_shadow_init returned error %d, aborting...", rc);
        abort();
        struct jsonStruct {
	const char *pKey; ///< JSON key
	void *pData; ///< pointer to the data (JSON value)
	size_t dataLength; ///< Length (in bytes) of pData
	JsonPrimitiveType type; ///< type of JSON
	jsonStructCallback_t cb; ///< callback to be executed on receiving the Key value pair
};
    }*/
    
    int value = 100;
    jsonStruct_t waterLevel {
        "water_level", &value, sizeof(int), SHADOW_JSON_INT32, NULL
    };
    waterLevel.cb = NULL;
    waterLevel.pKey = "water_level";
    waterLevel.pData = &value;
    waterLevel.type = SHADOW_JSON_INT32;
    waterLevel.dataLength = sizeof(int);
    /*ShadowConnectParameters_t scp = ShadowConnectParametersDefault;
    scp.pMyThingName = "waterlevel:cesena:1";
    scp.mqttClientIdLen = scp.mqttClientIdLen;
    scp.mqttClientIdLen = (uint16_t) strlen(scp.pMqttClientId);

    ESP_LOGI(TAG, "Shadow Connect");
    rc = aws_iot_shadow_connect(&mqttClient, &scp);
    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "aws_iot_shadow_connect returned error %d, aborting...", rc);
        vTaskDelay(portMAX_DELAY);
        abort();
    }*/
    
    char payload[200];
    aws_iot_shadow_init_json_document(payload, 200);
    aws_iot_shadow_add_reported(payload, 200, 1, &waterLevel);
    aws_iot_finalize_json_document(payload, 200);
    printf("DATA : = %s\n", payload);
    auto publishError = service.publish(payload);
    if(publishError != middleware::PUBLISHED) {
        printf("Error on publish..");
        abort();
    } else {
        printf("publish..");
    }
    auto error = service.disconnect();
    if(error != middleware::OK) {
        printf("Error on disconnection..");
        abort();
    }
    /*while(SUCCESS == rc) {
        rc = aws_iot_shadow_update(&mqttClient, scp.pMyThingName,payload,
                                    ShadowUpdateStatusCallback, NULL, 4, true); //false because we don't need the update of device
        shadowUpdateInProgress = true;
        printf("RC = %d", rc);

        aws_iot_shadow_yield(&mqttClient, 2000);
        ESP_LOGI(TAG, "*****************************************************************************************");
    }


    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "An error occurred in the loop %d", rc);
    }

    ESP_LOGI(TAG, "Disconnecting");
    rc = aws_iot_shadow_disconnect(&mqttClient);

    if(SUCCESS != rc) {
        ESP_LOGE(TAG, "Disconnect error %d", rc);
    }
    ESP_LOGI(TAG, "END...");
    vTaskDelay(portMAX_DELAY);
    */
    vTaskDelay(portMAX_DELAY);
    vTaskDelete(NULL);
}

extern "C" {
    
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {};
    memcpy(wifi_config.sta.ssid, EXAMPLE_WIFI_SSID, strlen(EXAMPLE_WIFI_PASS) + 1);
    memcpy(wifi_config.sta.password, EXAMPLE_WIFI_PASS, strlen(EXAMPLE_WIFI_PASS) + 1);
    
    
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_LOGI(TAG, "Setting WiFi configuration PASS %s...", wifi_config.sta.password);
    
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}
/*
void app_main()
{
    int value = 100;
    jsonStruct_t waterLevel {
        "water_level", &value, sizeof(int), SHADOW_JSON_INT32, NULL
    };
    char * prova = "{\"gps\": {\"bibo\":10}}";

    waterLevel.cb = NULL;
    waterLevel.pKey = "water_level";
    waterLevel.pData = prova;
    waterLevel.type = SHADOW_JSON_OBJECT;
    waterLevel.dataLength = strlen(prova) + 1;
    char payload[200];
    aws_iot_shadow_init_json_document(payload, 200);
    
    aws_iot_shadow_add_reported(payload, 200, 1, &waterLevel);
    printf("old DATA = %s\n", payload);
    fflush(stdout);
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    //wifi = new network::Wifi(EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    ESP_ERROR_CHECK( err );

    initialise_wifi();
    xTaskCreatePinnedToCore(&aws_iot_task, "aws_iot_task", 12000, NULL, 5, NULL, 1);
}*/