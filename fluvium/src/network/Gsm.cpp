#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "network/Gsm.h"
#include "sim800.h"

namespace network {
    const char* TAG = "GSM";
    const int CONNECT_BIT = BIT0;
    const int DISCONNECT_BIT = BIT0;
    const int CONNECTION_TIMEOUT_MS = 20;
}

using namespace network;

Gsm::Gsm(esp_modem_dte_config_t dteConfig, esp_ppp_config_t pppConfig) : dteConfig(dteConfig), pppConfig(pppConfig) {
    this->state = DISCONNECTED;
    tcpip_adapter_init();
    eventGroup = xEventGroupCreate();
}

NetworkState Gsm::getState() { return this->state; }

bool Gsm::connect() {
    if(state == CONNECTED) return true;

    if(!initModem()) {
        return false;
    }

    if(esp_modem_setup_ppp(dteHandler, &pppConfig) != ESP_OK) {
        return false;
    }
    
    bool timeout = xEventGroupWaitBits(eventGroup,
        CONNECT_BIT,
        pdTRUE,
        pdTRUE,
        pdMS_TO_TICKS(CONNECTION_TIMEOUT_MS)) && CONNECT_BIT != CONNECT_BIT;

    return !timeout & (state == CONNECTED);   
}

bool Gsm::disconnect() {
    if(state == DISCONNECTED) return true;
    if(esp_modem_exit_ppp(dteHandler) == ESP_OK) {
        xEventGroupWaitBits(eventGroup, DISCONNECT_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
        dceHandler->deinit(dceHandler);
        dteHandler->deinit(dteHandler);
        return true;
    }
    return false;
}

bool Gsm::standby() {
    ESP_LOGI(TAG, "TODO: standby not implemented\n");
    return true;
}

bool Gsm::initModem() {
    dteHandler = esp_modem_dte_init(&dteConfig);
    if(dteHandler == NULL) {
        ESP_LOGE(TAG, "Error during DTE initialization\n");
        return false;
    }

    esp_modem_add_event_handler(dteHandler, modemEventHandler, this);
    dceHandler = sim800_init(dteHandler);
    if(dceHandler == NULL) {
        ESP_LOGI(TAG, "Error during DCE sim800 initialization\n");
        return false;
    }

    dceHandler->set_flow_ctrl(dceHandler, MODEM_FLOW_CONTROL_NONE);
    dceHandler->store_profile(dceHandler);

    return true;
}

void Gsm::modemEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    Gsm* gsm = (Gsm*) event_handler_arg;
    switch (event_id) {
        case MODEM_EVENT_PPP_START:
            ESP_LOGI(TAG, "Modem PPP Started");
            break;
        case MODEM_EVENT_PPP_CONNECT: {
            ESP_LOGI(TAG, "Modem Connect to PPP Server");
            ppp_client_ip_info_t *ipinfo = (ppp_client_ip_info_t *)(event_data);
            ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
            ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&ipinfo->ip));
            ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&ipinfo->netmask));
            ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&ipinfo->gw));
            ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&ipinfo->ns1));
            ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&ipinfo->ns2));
            ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
            gsm->state = CONNECTED;
            xEventGroupClearBits(gsm->eventGroup, DISCONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, CONNECT_BIT);
            break;
        }
        case MODEM_EVENT_PPP_DISCONNECT:
            ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
            gsm->state = DISCONNECTED;
            xEventGroupClearBits(gsm->eventGroup, CONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, DISCONNECT_BIT);
            break;
        case MODEM_EVENT_PPP_STOP:
            ESP_LOGI(TAG, "Modem PPP Stopped");
            break;
        case MODEM_EVENT_UNKNOWN:
            ESP_LOGW(TAG, "Unknow line received: %s", (char *)event_data);
            break;
        default:
            break;
    }
}
