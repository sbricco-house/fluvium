#pragma once
#include "DCE.h"

using namespace dce;

namespace serial {
    struct SerialConfig {
        uart_port_t uartPort;
        gpio_num_t rxPin;
        gpio_num_t txPin;
        int baudrate;
    };

    class SerialDCE : public DCE {

        struct CommandResultHandler {
            Response response;
            ResponseHandler responseHandler;
        };

        private:
            SerialConfig config;
            Mode currentMode;
            DataHandler dataHandler;
            CommandResultHandler commandResult;
            QueueHandle_t uartQueue;
            EventGroupHandle_t eventGroup;
            SemaphoreHandle_t semaphore;

        public:
            SerialDCE(SerialConfig config);
            void registerDataHandler(DataHandler dataHandler) override;
            Response executeCommand(const char* command, ResponseHandler responseHandler, int timeoutMs = 2000) override;
            int sendRawData(const char* data, int length) override;
            bool switchMode(Mode mode) override;
    
        private:
            inline void createUartCommunication(SerialConfig config);
            static void uartEventHandler(void *pvParameters);
            static void handlePatternDetection(SerialDCE* dce, const char* data, int length);
    };
}