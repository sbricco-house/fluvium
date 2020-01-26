#include <str.h>
#include "middleware/AwsCoreService.h"
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
using namespace middleware;
#define AWS_TAG "aws_publisher"
#define NO_CALLBACK NULL
#define NO_CONTEXT_DATA NULL
AwsCoreService::AwsCoreService(char * deviceName, AwsPrivacyConfig privacyConfig, MqttConfig mqttHostConfig, AwsIotCoreConfig config) 
    : deviceName(deviceName), config(config) {
        shadowConnectionParams = createShadowConnectionParams(deviceName);
        shadowInitParameter = createShadowInitParams(mqttHostConfig, privacyConfig);
    }

//CREATE CONFIGURAITON DATA
ShadowInitParameters_t AwsCoreService::createShadowInitParams(MqttConfig mqttHostConfig, AwsPrivacyConfig privacyConfig) {
    ShadowInitParameters_t shadowParameter = ShadowInitParametersDefault;
    //shadow configuration
    shadowParameter.pHost = mqttHostConfig.name;
    shadowParameter.port = mqttHostConfig.port;
    shadowParameter.pClientCRT = privacyConfig.certificatePem;
    shadowParameter.pClientKey = privacyConfig.privateKey;
    shadowParameter.pRootCA = privacyConfig.awsRoot;
    shadowParameter.enableAutoReconnect = false;
    shadowParameter.disconnectHandler = NO_CALLBACK;
    return shadowParameter;
}
ShadowConnectParameters_t AwsCoreService::createShadowConnectionParams(char * deviceName) {
    auto shadowConnectionParams = ShadowConnectParametersDefault;
    shadowConnectionParams.pMyThingName = deviceName;
    shadowConnectionParams.pMqttClientId = deviceName;
    shadowConnectionParams.mqttClientIdLen = (uint16_t) strlen(deviceName);
    return shadowConnectionParams;
}

ConnectionResult AwsCoreService::connect() {
    
    auto error = aws_iot_shadow_init(&mqttClient, &shadowInitParameter);
    ESP_LOGI(AWS_TAG, "AWS INIT CODE : %d", error);
    if(error != SUCCESS) {
        return INITIALIZATION_PROBLEM;
    }
    error = aws_iot_shadow_connect(&mqttClient, &shadowConnectionParams);
    ESP_LOGI(AWS_TAG, "AWS CONNECT CODE : %d", error);
    return error != SUCCESS ? NO_CONNECTION : middleware::OK;
}

PublishResult AwsCoreService::publish(char * payload) {
    auto error = aws_iot_shadow_update(&mqttClient, deviceName, payload, NO_CALLBACK, NO_CONTEXT_DATA, config.maxTimeuot, false); //false because we don't need the update of device
    ESP_LOGI(AWS_TAG, "AWS PUBLISH CODE : %d", error);
    error = aws_iot_mqtt_yield(&mqttClient, 500);
    ESP_LOGI(AWS_TAG, "AWS YIELD CODE : %d", error);
    
    return error != SUCCESS ? PUBLISH_ERROR : PUBLISHED;
}


ConnectionResult AwsCoreService::disconnect() {
    auto error = aws_iot_shadow_disconnect(&mqttClient);
    ESP_LOGI(AWS_TAG, "AWS DISCONNECT CODE : %d", error);
    aws_iot_mqtt_free(&mqttClient);
    return error != SUCCESS ? DISCONNECTION_PROBLEM : OK;
}