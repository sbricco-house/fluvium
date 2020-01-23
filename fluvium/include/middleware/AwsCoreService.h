#pragma once 
#include "MiddlewareService.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
#include "Util.h"
namespace middleware {
    struct AwsPrivacyConfig {
        const char * certificatePem;
        const char * privateKey;
        const char * awsRoot;
        AwsPrivacyConfig(const char * certificatePem, const char * privateKey,const char * awsRoot) :
            certificatePem(certificatePem), privateKey(privateKey), awsRoot(awsRoot) {}
    };
    struct MqttConfig {
        char * name;
        const int port;
        MqttConfig(char * name, int port = 8883) : name(name), port(port) { }
    };

    struct AwsIotCoreConfig {
        uint8_t maxTimeuot;
    };
    class AwsCoreService : public MiddlewareService {
        private:
            char * deviceName;
            AWS_IoT_Client mqttClient;
            ShadowInitParameters_t shadowInitParameter;
            ShadowConnectParameters_t shadowConnectionParams;
            AwsIotCoreConfig config;
        public:
            AwsCoreService(char * deviceName, AwsPrivacyConfig privacyConfig, MqttConfig mqttHostName, AwsIotCoreConfig config);
            ConnectionResult connect() override;
            ConnectionResult reconnect() override;
            ConnectionResult disconnect() override;
            PublishResult publish(char * payload) override;
        private:
            inline ShadowInitParameters_t createShadowInitParams(MqttConfig mqttHostConfig, AwsPrivacyConfig config);
            inline ShadowConnectParameters_t createShadowConnectionParams(char * deviceName);
        
    };
}