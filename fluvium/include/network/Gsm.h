#include "Network.h"
#include "freertos/event_groups.h"
#include "esp_modem.h"

namespace network {    
    class Gsm : public Network {
        private:
            NetworkState state;
            EventGroupHandle_t eventGroup;
            modem_dte_t* dteHandler;
            modem_dce_t* dceHandler;
            esp_modem_dte_config_t dteConfig;
            esp_ppp_config_t pppConfig;
        public:
            Gsm(esp_modem_dte_config_t dteConfig, esp_ppp_config_t pppConfig);
            NetworkState getState() override;
            bool connect() override;
            bool disconnect() override;
            bool standby() override;
        private:
            static void modemEventHandler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
            bool initModem();
    };
}