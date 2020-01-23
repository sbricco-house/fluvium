#pragma once

#include <iostream>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Util.h"
#include <aws_iot_json_utils.h>
#include <aws_iot_shadow_json.h>

namespace buffering {
    using data_type = uint8_t;
    using json = std::string;

    template <typename DATA_TYPE> 
    static jsonStruct_t jsonCreate(char * nameKey, DATA_TYPE value, JsonPrimitiveType jsonType);

    struct Data {
        data_type id; 
        timeutils::millisecond timestamp;

        Data(data_type id) {
            this->timestamp = timeutils::timestampMs();
            this->id = id;
        }
    };

    class Buffer {
        private:
            const QueueHandle_t xQueue;
        public:
            Buffer(int size);
            bool queue(const Data* data) const;
            void clear() const;
            Data* dequeue() const;
            bool isEmpty() const;
    };

    class Parser {
        private: 
            data_type parsingType;
        public:
            Parser(data_type parsingType);
            /*format output: json*/
            json serialize(const Data& data);

        protected:
            virtual json doSerialize(const Data& data) = 0;
    };
}