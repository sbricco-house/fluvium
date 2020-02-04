#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "network/Gsm.h"

namespace network {
    const char* TAG = "GSM";
    const int CONNECTING_BIT = BIT0;
    const int CONNECT_BIT = BIT1;
    const int DISCONNECT_BIT = BIT2;
}

using namespace network;

// internal functions
static void printNetworkInfo(modem::ModemIpInfo * ipinfo);

Gsm::Gsm(modem::Modem& modem) : modem(modem), eventGroup(xEventGroupCreate()) {
    tcpip_adapter_init();
    initModem();
}

Gsm::~Gsm() {
    // free all allocated resources
    modem.unregisterEventHandler(modemEventHandler);
    //modem.stop();
    /*esp_modem_stop_ppp(dteHandler);
    dteHandler->deinit(dteHandler);
    dceHandler->deinit(dceHandler);*/
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
    return false;
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
    return false;
}

bool Gsm::startConnect() {
    return modem.connect();
}

bool Gsm::waitConnectionDone() {
    return (xEventGroupWaitBits(eventGroup,
        CONNECT_BIT | DISCONNECT_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY) & CONNECT_BIT) == CONNECT_BIT;
}

bool Gsm::startDisconnect() {
    return modem.disconnect();
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
}

bool Gsm::initModem() {
    modem.registerEventHandler(modemEventHandler, this);
    return modem.start();
}

void Gsm::modemEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    Gsm* gsm = (Gsm*) event_handler_arg;
    switch (event_id) {
        case PPP_START:
            ESP_LOGI(TAG, "Modem PPP Started");
            xEventGroupClearBits(gsm->eventGroup, DISCONNECT_BIT | CONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, CONNECTING_BIT);
            break;
        case PPP_CONNECTED: {
            ESP_LOGI(TAG, "Modem Connect to PPP Server");
            printNetworkInfo(static_cast<modem::ModemIpInfo *>(event_data));
            xEventGroupClearBits(gsm->eventGroup, CONNECTING_BIT | DISCONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, CONNECT_BIT);
            break;
        }
        case PPP_DISCONNECTED:
        case PPP_STOP:
            ESP_LOGI(TAG, "%s", (event_id == PPP_STOP) ? "Modem PPP Stopped" : "Modem Disconnect from PPP Server");
            xEventGroupClearBits(gsm->eventGroup, CONNECTING_BIT | CONNECT_BIT);
            xEventGroupSetBits(gsm->eventGroup, DISCONNECT_BIT); 
            break;
        default:
            break;
    }
}

static void printNetworkInfo(modem::ModemIpInfo* ipinfo) {
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&ipinfo->ip));
    ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&ipinfo->netmask));
    ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&ipinfo->gw));
    ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&ipinfo->ns1));
    ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&ipinfo->ns2));
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
}