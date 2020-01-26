#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "network/Gsm.h"
#include "sim800.h"
#include <driver/gpio.h>

namespace network {
    const char* TAG = "GSM";
    const int CONNECTING_BIT = BIT0;
    const int CONNECT_BIT = BIT1;
    const int DISCONNECT_BIT = BIT2;
    const int INIT_TIMEOUT_MS = 5000;
}

using namespace network;

// internal functions
static void printNetworkInfo(ppp_client_ip_info_t * ipinfo);

Gsm::Gsm(const GsmConfig config) : config(config), eventGroup(xEventGroupCreate()) {
    tcpip_adapter_init();
    gpio_pad_select_gpio(config.resetPin); // RST
    gpio_pad_select_gpio(config.powerKeyPin); // PW KEY
    gpio_pad_select_gpio(config.powerOnPin); // POWER_ON
    gpio_set_direction(config.resetPin, GPIO_MODE_OUTPUT);
    gpio_set_direction(config.powerKeyPin, GPIO_MODE_OUTPUT);
    gpio_set_direction(config.powerOnPin, GPIO_MODE_OUTPUT);
    initModem();
}

Gsm::~Gsm() {
    // free all allocated resources
    esp_modem_stop_ppp(dteHandler);
    dteHandler->deinit(dteHandler);
    dceHandler->deinit(dceHandler);
    vEventGroupDelete(eventGroup);
}

NetworkState Gsm::getState() { 
    auto bits = xEventGroupGetBits(eventGroup);
    if((bits & CONNECTING_BIT) == CONNECTING_BIT) {
        return NetworkState::CONNECTING;
    }
    else if((bits & CONNECT_BIT) == CONNECT_BIT) {
        return NetworkState::CONNECTED;
    }
    else if((bits & DISCONNECT_BIT) == DISCONNECT_BIT) {
        return NetworkState::DISCONNECTED;
    }
    return NetworkState::DISCONNECTED;
}

bool Gsm::connect() {
    auto state = getState();
    switch (state)
    {
        case CONNECTED:
            return true;
        case DISCONNECTED: {
            return startConnect() ? waitConnectionDone() : false;
        }
        case CONNECTING:
            return waitConnectionDone();
        default:
            return false;
    }
}

bool Gsm::disconnect() {
    auto state = getState();
    switch (state)
    {
        case DISCONNECTED:
            return true;
        case CONNECTING:
        case CONNECTED: {
            return startDisconnect() ? waitDisconnectionDone() : false;
        }
        default:
            return false;
    }
}

bool Gsm::startConnect() {
    if(esp_modem_connect_ppp(dteHandler, &config.pppConfig) != ESP_OK) {
        printf("Start connect fail\n");
        return false;
    }
    printf("Start connect success\n");
    return true;
}

bool Gsm::waitConnectionDone() {
    return (xEventGroupWaitBits(eventGroup,
        CONNECT_BIT | DISCONNECT_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY) & CONNECT_BIT) == CONNECT_BIT;
}

bool Gsm::startDisconnect() {
    ESP_LOGI(TAG, "Start disconnection\n");
    esp_modem_disconnect_ppp(dteHandler);
    return true;
}

bool Gsm::waitDisconnectionDone() {
    return (xEventGroupWaitBits(eventGroup,
        DISCONNECT_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY) & DISCONNECT_BIT) == DISCONNECT_BIT;
}

// https://www.raviyp.com/sim900-sim800-sleep-mode-at-commands/
bool Gsm::standby() {
    // Enter to power save mode
    // 1. disconnect GPRS
    //if(!disconnect()) { return false; }
    // 2. DCE enter to powersave mode
    //if(dceHandler->enter_powersave(dceHandler) != ESP_OK) { return false; }
    //state = STANDBY;
    ESP_LOGD(TAG, "TODO: implement using AT+CFUN\n");
    return true;
    return true;
}

bool Gsm::initModem() {
    powerOnModem();

    vTaskDelay(pdMS_TO_TICKS(INIT_TIMEOUT_MS));

    dteHandler = esp_modem_dte_init(&config.dteConfig);
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

    return esp_modem_start_ppp(dteHandler) == ESP_OK;
}

void Gsm::powerOnModem() {
    gpio_set_level(config.powerKeyPin, 0);
    gpio_set_level(config.resetPin, 1);
    gpio_set_level(config.powerOnPin, 1);
}

void Gsm::modemEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    Gsm* gsm = (Gsm*) event_handler_arg;
    switch (event_id) {
        case MODEM_EVENT_PPP_START:
            ESP_LOGI(TAG, "Modem PPP Started");
            xEventGroupClearBits(gsm->eventGroup, DISCONNECT_BIT | CONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, CONNECTING_BIT);
            break;
        case MODEM_EVENT_PPP_CONNECT: {
            ESP_LOGI(TAG, "Modem Connect to PPP Server");
            printNetworkInfo(static_cast<ppp_client_ip_info_t *>(event_data));
            xEventGroupClearBits(gsm->eventGroup, CONNECTING_BIT | DISCONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, CONNECT_BIT);
            break;
        }
        case MODEM_EVENT_PPP_DISCONNECT: {
                ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
                // the sim800 library dont switch to AT command mode, force disconnect
                // in order to force the switch to AT command mode.
                gsm->startDisconnect(); // force a user interrupt
            }
            break;
        case MODEM_EVENT_PPP_STOP:
            ESP_LOGI(TAG, "Modem PPP Stopped");
            xEventGroupClearBits(gsm->eventGroup, CONNECTING_BIT | DISCONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, DISCONNECT_BIT);
            break;
        case MODEM_EVENT_UNKNOWN:
            ESP_LOGW(TAG, "Unknow line received: %s", (char *)event_data);
            break;
        default:
            break;
    }
}

static void printNetworkInfo(ppp_client_ip_info_t * ipinfo) {
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&ipinfo->ip));
    ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&ipinfo->netmask));
    ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&ipinfo->gw));
    ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&ipinfo->ns1));
    ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&ipinfo->ns2));
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
}