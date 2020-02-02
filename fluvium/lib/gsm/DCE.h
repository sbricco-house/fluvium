#pragma once

#include "driver/uart.h"
#include <types.h>
#include <functional>
#include "freertos/event_groups.h"


namespace dce {
    
    struct Response {
        bool isSuccess;
        std::string response;
        
        public:
            static Response success(std::string response) {
                return Response { true, response };
            };
            static Response fail(std::string response = "") {
                return Response { false, response };
            }
    };

    using ResponseHandler = std::function<Response(const char* response, int length)>;
    using DataHandler = std::function<void(const unsigned char* data, int length)>;

    enum Mode {
        COMMAND,
        DATA
    };

    class DCE {
        public:
            virtual bool switchMode(Mode mode) = 0;
            virtual Response executeCommand(const char* command, ResponseHandler responseHandler, int timeoutMs = 2000) = 0;
            virtual int sendRawData(const char* data, int length) = 0;
            virtual void registerDataHandler(DataHandler dataHandler) = 0;
    };
}