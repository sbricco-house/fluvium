#include "Sim800.h"
#include <cstring>
#include <stdlib.h>

#include "wrap/WrapPPP.h"
#include "parser/ATCommandParser.h"

ESP_EVENT_DEFINE_BASE(MODEM_EVENT)

#define EVENT_LOOP_QUEUE_SIZE 20
#define MODEM_COMMAND_TIMEOUT_HANG_UP 90000

using namespace modem;

Sim800::Sim800(Sim800Config config, dce::DCE& dce) 
    : config(config), dce(dce) {
        esp_event_loop_args_t loopArgs = {
            .queue_size = EVENT_LOOP_QUEUE_SIZE,
            .task_name = "modem_event_loop",
            .task_priority = 5,
            .task_stack_size = 2048,
            .task_core_id = tskNO_AFFINITY
        };
        esp_event_loop_create(&loopArgs, &eventloop);
};

void Sim800::registerEventHandler(ModemEventHandler eventHandler, Context ctx) {
    esp_event_handler_register_with(eventloop, MODEM_EVENT, ESP_EVENT_ANY_ID, eventHandler, ctx);
}

void Sim800::unregisterEventHandler(ModemEventHandler eventHandler) {
    esp_event_handler_unregister_with(eventloop, MODEM_EVENT, ESP_EVENT_ANY_ID, eventHandler);
}

bool Sim800::start() {
    turnOnModule();
    
    tcpip_adapter_init();
    ppp = pppapi_pppos_create(&pppif, pppOutputHandler, pppStatusChangeHandler, this);
    if(ppp == nullptr) return false;
    
    pppapi_set_default(ppp);
    ppp_set_usepeerdns(ppp, 1);

    dce.registerDataHandler([&](const unsigned char* data, int length) {
        if(length) {
            ppp_input_to_tcpip(ppp, (u8_t*) data, length);
        }
    });
    return true;
}

bool Sim800::connect() {
    if(enablePPP(config.apn)) {
        esp_event_post_to(eventloop, MODEM_EVENT, PPP_START, NULL, 0, portMAX_DELAY);
        pppapi_set_auth(ppp, PPPAUTHTYPE_PAP, config.username, config.password);
        return pppapi_connect(ppp, 0) == ERR_OK;
    }
    return false;
}

bool Sim800::disconnect() {
    pppapi_close(ppp, 1);
    return enableCommand();
}

bool Sim800::enablePPP(const char* apn) {
    char buff[64];
    sprintf(buff, "AT+CGDCONT=%d, \"%s\", \"%s\"\r\n", 1, "IP", apn);

    auto response = dce.executeCommand(buff, defaultResponseHandler);
    if(!response.isSuccess) return false;
    response = dce.executeCommand("ATD*99#\r\n", connectResponseHandler);
    if(!response.isSuccess) return false;

    // now enable data flowing from uart
    return dce.switchMode(dce::Mode::DATA);
}

bool Sim800::enableCommand() {
    if(dce.switchMode(dce::Mode::COMMAND)) {
        vTaskDelay(pdMS_TO_TICKS(2000)); // wait before send +++, to avoid as DATA interpretation
        auto response = dce.executeCommand("+++", exitResponseHandler);
        response = dce.executeCommand("ATH\r\n", defaultResponseHandler, MODEM_COMMAND_TIMEOUT_HANG_UP);
        return response.isSuccess;
    }
    return false;
}

/**
 * Send raw data from PPP layer to UART serial
 **/
uint32_t Sim800::pppOutputHandler(ppp_pcb *pcb, uint8_t *data, uint32_t len, void *ctx) {
    Sim800* modem = static_cast<Sim800*>(ctx);
    return modem->dce.sendRawData((const char*) data, len);
}

/**
 * Handler for any PPP status change, it handle any disconnection error.
 * It post a set of simplified event to the event loop.
 **/
void Sim800::pppStatusChangeHandler(ppp_pcb *pcb, int err_code, void *ctx) {
    struct netif *pppif = ppp_netif(pcb);
    Sim800* modem = static_cast<Sim800*>(ctx);
    ModemIpInfo ipInfo;
    switch(err_code) {
        case PPPERR_NONE: /* Is connected */
            ipInfo.ip = pppif->ip_addr.u_addr.ip4;
            ipInfo.gw = pppif->gw.u_addr.ip4;
            ipInfo.netmask = pppif->netmask.u_addr.ip4;
            ipInfo.ns1 = dns_getserver(0).u_addr.ip4;
            ipInfo.ns2 = dns_getserver(1).u_addr.ip4;
            esp_event_post_to(modem->eventloop, MODEM_EVENT, PPP_CONNECTED, &ipInfo, sizeof(ipInfo), portMAX_DELAY);
            break;
        case PPPERR_USER: /* User interrupt */
            esp_event_post_to(modem->eventloop, MODEM_EVENT, PPP_STOP, NULL, 0, portMAX_DELAY);
            break;
        case PPPERR_CONNECT: /* Connection lost */
            modem->enableCommand();
            esp_event_post_to(modem->eventloop, MODEM_EVENT, PPP_DISCONNECTED, NULL, 0, portMAX_DELAY);
            break;
        default:
            ESP_LOGE("Modem", "Error! Code: %d\n", err_code);
            break;
    }
}

void Sim800::turnOnModule() {
    gpio_pad_select_gpio(config.resetPin); // RST
    gpio_pad_select_gpio(config.powerKeyPin); // PW KEY
    gpio_pad_select_gpio(config.powerOnPin); // POWER_ON
    gpio_set_direction(config.resetPin, GPIO_MODE_OUTPUT);
    gpio_set_direction(config.powerKeyPin, GPIO_MODE_OUTPUT);
    gpio_set_direction(config.powerOnPin, GPIO_MODE_OUTPUT);
    gpio_set_level(config.powerKeyPin, 0);
    gpio_set_level(config.resetPin, 1);
    gpio_set_level(config.powerOnPin, 1);
    vTaskDelay(pdMS_TO_TICKS(4000)); // wait turning on
}