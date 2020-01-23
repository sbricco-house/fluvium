#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "sim800.h"
#include "esp_ping.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define TAG "GSM"

#define CONNECT_BIT BIT0

extern "C" void app_main();
extern "C" void modem_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void testSocketGoogle();

EventGroupHandle_t eventGroup = xEventGroupCreate();

void app_main() {
    tcpip_adapter_init();

    // create DTE object
    esp_modem_dte_config_t config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    config.port_num = UART_NUM_2;
    config.rx_pin = GPIO_NUM_16;
    config.tx_pin = GPIO_NUM_17;

    // change rx and tx pin
    modem_dte_t *dte = esp_modem_dte_init(&config);
    if(dte == NULL) {
        ESP_LOGE(TAG, "Error during dte creation\n");
        return;
    }

    esp_modem_add_event_handler(dte, modem_event, NULL);
    modem_dce_t *dce = sim800_init(dte);

    if(dce == NULL) {
        ESP_LOGI(TAG, "Error during dce creation");
        return;
    }

    dce->set_flow_ctrl(dce, MODEM_FLOW_CONTROL_NONE);
    dce->store_profile(dce);
    // Print Module ID, Operator, IMEI, IMSI
    ESP_LOGI(TAG, "Module: %s", dce->name);
    ESP_LOGI(TAG, "Operator: %s", dce->oper);
    ESP_LOGI(TAG, "IMEI: %s", dce->imei);
    ESP_LOGI(TAG, "IMSI: %s", dce->imsi);

    // Get signal quality 
    uint32_t rssi = 0, ber = 0;
    ESP_ERROR_CHECK(dce->get_signal_quality(dce, &rssi, &ber));
    ESP_LOGI(TAG, "rssi: %d, ber: %d", rssi, ber);

    // Get battery voltage 
    uint32_t voltage = 0, bcs = 0, bcl = 0;
    ESP_ERROR_CHECK(dce->get_battery_status(dce, &bcs, &bcl, &voltage));
    ESP_LOGI(TAG, "Battery voltage: %d mV", voltage);

    // Setup PPP environment 
    esp_ppp_config_t pppConfig = {
        .apn_name = "internet.wind",
        .ppp_auth_username = "",
        .ppp_auth_password = ""
    };

    esp_modem_setup_ppp(dte, &pppConfig);

    xEventGroupWaitBits(eventGroup, BIT0, pdTRUE, pdTRUE, portMAX_DELAY);
    testSocketGoogle();

    // vTaskDelay(pdMS_TO_TICKS(100000));

    esp_modem_exit_ppp(dte);

    //Power down module
    ESP_ERROR_CHECK(dce->power_down(dce));
    ESP_LOGI(TAG, "Power down");
    ESP_ERROR_CHECK(dce->deinit(dce));
    ESP_ERROR_CHECK(dte->deinit(dte));
}

void testSocketGoogle() {
    int socket_desc;
	struct sockaddr_in server;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
		
	server.sin_addr.s_addr = inet_addr("140.82.118.3");
	server.sin_family = AF_INET;
	server.sin_port = htons( 80 );

	//Connect to remote server
	int sock = connect(socket_desc , (struct sockaddr *)&server , sizeof(server));
    if (sock < 0)
	{
		ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
		return;
	}

    ESP_LOGI(TAG, "Successfully connected");

    ESP_LOGE(TAG, "Shutting down socket and restarting...");
    shutdown(sock, 0);
    close(sock);
}

void modem_event(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
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
            xEventGroupSetBits(eventGroup, CONNECT_BIT);
            break;
        }
        case MODEM_EVENT_PPP_DISCONNECT:
            ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
            break;
        case MODEM_EVENT_PPP_STOP:
            ESP_LOGI(TAG, "Modem PPP Stopped");
            //xEventGroupSetBits(event_group, STOP_BIT);
            break;
        case MODEM_EVENT_UNKNOWN:
            ESP_LOGW(TAG, "Unknow line received: %s", (char *)event_data);
            break;
        default:
            break;
    }
}