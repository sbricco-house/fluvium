#include "esp_log.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "network/Gsm.h"

#define TAG "Main"

//extern "C" void app_main();
void testSocketGoogle();

void app_main() {
    esp_modem_dte_config_t config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    config.port_num = UART_NUM_2;
    config.rx_pin = GPIO_NUM_16;
    config.tx_pin = GPIO_NUM_17;

    esp_ppp_config_t pppConfig = {
        .apn_name = "internet.wind",
        .ppp_auth_username = "",
        .ppp_auth_password = ""
    };

    network::Gsm gsm(config, pppConfig);

    ESP_LOGI(TAG, "Connecting...\n");
    gsm.connect(); // blocking connect method
    ESP_LOGI(TAG, "Connected!\n");
    
    testSocketGoogle();

    ESP_LOGI(TAG, "Disconnect...\n");
    gsm.disconnect();
    ESP_LOGI(TAG, "Disconnected!\n");
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