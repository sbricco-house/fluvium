#include "esp_log.h"
#include "esp_system.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "network/Gsm.h"
#include "network/NetworkFactory.h"

#define TAG "Main"
#define RECONNECT_ATTEMPS 5

//extern "C" void app_main();
void testSocketGoogle();

/*void app_main() {
    
    auto gsm = networkfactory::createGsmTTGO("iliad");

    ESP_LOGI(TAG, "Connecting...\n");
    bool connected = false;
    for(int i = 0; i < RECONNECT_ATTEMPS && !connected; i++, connected = gsm.connect()) {}
    
    if(connected) {
        testSocketGoogle();
        bool disconnected = gsm.disconnect();
        printf("%s\n", disconnected ? "Disconnected" : "Fail to disconnect");
    } else {
        printf("Connection failed after %d times\n", RECONNECT_ATTEMPS);
    }
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
}*/