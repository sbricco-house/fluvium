#include "SerialDCE.h"

#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"

namespace serial { const char* TAG = "SerialDCE"; }
using namespace serial;

#define RX_BUF_SIZE 2048
#define TX_BUF_SIZE 0 // no buffer for send
#define PATTERN_BUF_SIZE 20

#define COMMAND_BIT BIT0


SerialDCE::SerialDCE(SerialConfig config) :
        config(config), currentMode((Mode)-1), eventGroup(xEventGroupCreate()), semaphore(xSemaphoreCreateMutex())
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    createUartCommunication(config);
    switchMode(COMMAND);
}

void SerialDCE::registerDataHandler(DataHandler dataHandler) {
    this->dataHandler = dataHandler;
}

int SerialDCE::sendRawData(const char* data, int length) {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    Mode mode = currentMode;
    xSemaphoreGive(semaphore);

    return (mode == DATA) ? 
        uart_write_bytes(config.uartPort, (const char*) data, length) :
        -1;
}

Response SerialDCE::executeCommand(const char* command, ResponseHandler responseHandler, int timeoutMs) {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    this->commandResult.responseHandler = responseHandler;
    xSemaphoreGive(semaphore);
    
    uart_write_bytes(config.uartPort, command, strlen(command));
    xEventGroupWaitBits(eventGroup, COMMAND_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(timeoutMs));
    
    xSemaphoreTake(semaphore, portMAX_DELAY);
    Response response = this->commandResult.response;
    this->commandResult.responseHandler = nullptr;
    this->commandResult.response.isSuccess = false;
    xSemaphoreGive(semaphore);
    
    return response;
}

void SerialDCE::handlePatternDetection(SerialDCE* dce, const char* data, int length) {
    ESP_LOGD(TAG, "Pattern detect line: %s\n", data);
    xSemaphoreTake(dce->semaphore, portMAX_DELAY);
    if(dce->commandResult.responseHandler != nullptr) {
        /* Skip pure "\r\n" lines */
        if(length > 2) {
            auto resp = dce->commandResult.responseHandler(data, length);
            dce->commandResult.response = resp;
            if(resp.isSuccess) {
                xEventGroupSetBits(dce->eventGroup, COMMAND_BIT);
            }
        }    
    } else {
        ESP_LOGD(TAG, "No response handler for %s\n", data);
    }
    xSemaphoreGive(dce->semaphore);
}

bool SerialDCE::switchMode(Mode mode) {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    if(currentMode == mode) { 
        ESP_LOGW(TAG, "Already in mode %s\n", (mode == DATA ? "Data" : "Command"));
        xSemaphoreGive(semaphore);
        return false; 
    }
    switch (mode)
    {
        case COMMAND: // disable RX interrupt and enable pattern interrupt
            currentMode = COMMAND;
            uart_disable_rx_intr(config.uartPort);
            uart_flush(config.uartPort);
            uart_enable_pattern_det_intr(config.uartPort, '\n', 1, 9, 0, 0);
            uart_pattern_queue_reset(config.uartPort, PATTERN_BUF_SIZE);
            break;
        case DATA:
            currentMode = DATA;
            uart_disable_pattern_det_intr(config.uartPort);
            uart_enable_rx_intr(config.uartPort);
            break;
    }
    xSemaphoreGive(semaphore);
    return true;
}

void SerialDCE::uartEventHandler(void *pvParameters) {
    SerialDCE* serial = static_cast<SerialDCE*>(pvParameters);
    uint8_t buffer[RX_BUF_SIZE];
    uart_event_t event;
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(serial->uartQueue, (void *)&event, (portTickType)portMAX_DELAY)) {
            bzero(buffer, RX_BUF_SIZE); // clear the buffer

            switch (event.type)
            {
                case UART_DATA: {
                    size_t length = 0;
                    uart_get_buffered_data_len(serial->config.uartPort, &length);   
                    length = uart_read_bytes(serial->config.uartPort, buffer, length, portMAX_DELAY);
                    xSemaphoreTake(serial->semaphore, portMAX_DELAY);
                    if(serial->dataHandler != NULL) {
                        serial->dataHandler(buffer, length);
                    }
                    xSemaphoreGive(serial->semaphore);
                    break;
                }
                case UART_PATTERN_DET: {
                    int pos = uart_pattern_pop_pos(serial->config.uartPort);
                    int read_len = 0;
                    if (pos != -1) {
                        if (pos < RX_BUF_SIZE - 1) {
                            /* read one line(include '\n') */
                            read_len = pos + 1;
                        } else {
                            ESP_LOGW(TAG, "ESP Modem Line buffer too small");
                            read_len = RX_BUF_SIZE - 1;
                        }
                        read_len = uart_read_bytes(serial->config.uartPort, buffer, read_len, pdMS_TO_TICKS(100));
                        if (read_len) {
                            /* make sure the line is a standard string */
                            buffer[read_len] = '\0';
                            /* Send new line to handle */
                            handlePatternDetection(serial, (const char*) buffer, read_len);
                        } else {
                            ESP_LOGW(TAG, "uart read bytes failed");
                        }
                    } else {
                        ESP_LOGW(TAG, "Pattern Queue Size too small");
                        uart_flush(serial->config.uartPort);
                    }
                    break;
                }
                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    uart_flush_input(serial->config.uartPort);
                    xQueueReset(serial->uartQueue);
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGD(TAG, "uart parity error");
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGD(TAG, "uart frame error");
                    break;
                default:
                    ESP_LOGD(TAG, "uart event type: %d", event.type);
                    break;
            }

        }
    }

    vTaskDelete(NULL);
}

// CREATE AND SETUP AN UART COMMUNICATION LAYER
void SerialDCE::createUartCommunication(SerialConfig config) {
    uart_config_t uartConfig;
    uartConfig.baud_rate = config.baudrate;
    uartConfig.data_bits = UART_DATA_8_BITS;
    uartConfig.parity = UART_PARITY_DISABLE;
    uartConfig.stop_bits = UART_STOP_BITS_1;
    uartConfig.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    uart_param_config(config.uartPort, &uartConfig);
    uart_set_pin(config.uartPort, config.txPin, config.rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(config.uartPort, RX_BUF_SIZE, TX_BUF_SIZE, PATTERN_BUF_SIZE, &uartQueue, 0);

    xTaskCreate(uartEventHandler, "uart_event_handler_task", 4096, this, 2, NULL);
}