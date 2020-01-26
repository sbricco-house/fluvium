#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "modem_dce.h"
#include "modem_dte.h"
#include "esp_event.h"
#include "driver/uart.h"

/**
 * @brief Declare Event Base for ESP Modem
 *
 */
ESP_EVENT_DECLARE_BASE(ESP_MODEM_EVENT);
/**
 * @brief ESP Modem Event
 *
 */
typedef enum {
    MODEM_EVENT_PPP_START,      /*!< ESP Modem Start PPP Session */
    MODEM_EVENT_PPP_CONNECT,    /*!< ESP Modem Connect to PPP Server */
    MODEM_EVENT_PPP_DISCONNECT, /*!< ESP Modem Disconnect from PPP Server */
    MODEM_EVENT_PPP_STOP,       /*!< ESP Modem Stop PPP Session*/
    MODEM_EVENT_UNKNOWN         /*!< ESP Modem Unknown Response */
} esp_modem_event_t;

/**
 * @brief ESP Modem DTE Configuration
 *
 */
typedef struct {
    uart_port_t port_num;           /*!< UART port number */
    gpio_num_t rx_pin;              /*!< RX pin number */
    gpio_num_t tx_pin;              /*!< TX pin number */
    uart_word_length_t data_bits;   /*!< Data bits of UART */
    uart_stop_bits_t stop_bits;     /*!< Stop bits of UART */
    uart_parity_t parity;           /*!< Parity type */
    modem_flow_ctrl_t flow_control; /*!< Flow control type */
    gpio_num_t cts_pin;             /*!< CTS pin if hardware flow control is enabled */
    gpio_num_t rts_pin;             /*!< RTS pin if hardware flow control is enabled */
    uint32_t baud_rate;             /*!< Communication baud rate */
} esp_modem_dte_config_t;

typedef struct {
    const char* apn_name;
    const char* ppp_auth_username;
    const char* ppp_auth_password;
} esp_ppp_config_t;

#define ESP_MODEM_PPP_DEFAULT_CONFIG(apn)       \
    {                                           \
        .apn_name = apn,                        \
        .ppp_auth_username = "",                \
        .ppp_auth_password = ""                 \
    }

/**
 * @brief ESP Modem DTE Default Configuration
 *
 */
#define ESP_MODEM_DTE_DEFAULT_CONFIG()          \
    {                                           \
        .port_num = UART_NUM_1,                 \
        .rx_pin = GPIO_NUM_16,                  \
        .tx_pin = GPIO_NUM_17,                  \
        .data_bits = UART_DATA_8_BITS,          \
        .stop_bits = UART_STOP_BITS_1,          \
        .parity = UART_PARITY_DISABLE,          \
        .flow_control = MODEM_FLOW_CONTROL_NONE,\
        .cts_pin = (gpio_num_t) UART_PIN_NO_CHANGE,          \
        .rts_pin = (gpio_num_t) UART_PIN_NO_CHANGE,          \
        .baud_rate = 115200                     \
    }

/**
 * @brief Create and initialize Modem DTE object
 *
 * @param config configuration of ESP Modem DTE object
 * @return modem_dte_t*
 *      - Modem DTE object
 */
modem_dte_t *esp_modem_dte_init(const esp_modem_dte_config_t *config);

/**
 * @brief Register event handler for ESP Modem event loop
 *
 * @param dte modem_dte_t type object
 * @param handler event handler to register
 * @param handler_args arguments for registered handler
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM on allocating memory for the handler failed
 *      - ESP_ERR_INVALID_ARG on invalid combination of event base and event id
 */
esp_err_t esp_modem_add_event_handler(modem_dte_t *dte, esp_event_handler_t handler, void *handler_args);

/**
 * @brief Unregister event handler for ESP Modem event loop
 *
 * @param dte modem_dte_t type object
 * @param handler event handler to unregister
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG on invalid combination of event base and event id
 */
esp_err_t esp_modem_remove_event_handler(modem_dte_t *dte, esp_event_handler_t handler);

/**
 * @brief PPPoS Client IP Information
 *
 */
typedef struct {
    ip4_addr_t ip;      /*!< IP Address */
    ip4_addr_t netmask; /*!< Net Mask */
    ip4_addr_t gw;      /*!< Gateway */
    ip4_addr_t ns1;     /*!< Name Server1 */
    ip4_addr_t ns2;     /*!< Name Server2 */
} ppp_client_ip_info_t;

esp_err_t esp_modem_start_ppp(modem_dte_t *dte);

esp_err_t esp_modem_connect_ppp(modem_dte_t *dte, esp_ppp_config_t *ppp_config);

esp_err_t esp_modem_disconnect_ppp(modem_dte_t *dte);

esp_err_t esp_modem_stop_ppp(modem_dte_t *dte);

#ifdef __cplusplus
}
#endif