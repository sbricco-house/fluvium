#include <str.h>
#include "middleware/AwsCoreService.h"
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
using namespace middleware;
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
    if(error != SUCCESS) {
        return INITIALIZATION_PROBLEM;
    }
    error = aws_iot_shadow_connect(&mqttClient, &shadowConnectionParams);
    return error != SUCCESS ? NO_CONNECTION : middleware::OK;
}

ConnectionResult AwsCoreService::reconnect() {
    auto error = aws_iot_mqtt_attempt_reconnect(&mqttClient);
    return error != SUCCESS ? NO_CONNECTION : middleware::OK;
}


PublishResult AwsCoreService::publish(char * payload) {
    ESP_LOGI("aws_shadow", "publishing.. : %s", payload);
    auto error = aws_iot_shadow_update(&mqttClient, deviceName, payload, NO_CALLBACK, NO_CONTEXT_DATA, config.maxTimeuot, false); //false because we don't need the update of device
    return error != SUCCESS ? PUBLISH_ERROR : PUBLISHED;
}


ConnectionResult AwsCoreService::disconnect() {
    auto error = aws_iot_shadow_disconnect(&mqttClient);
    return error != SUCCESS ? DISCONNECTION_PROBLEM : OK;
}