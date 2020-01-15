#pragma once
#include <iostream>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "Util.h"
namespace buffering {
    using data_type = uint8_t;
    using csv = std::string;
    struct Data { 
        data_type id; 
        long timestap;

        Data(data_type id) {
            this->timestap = timeutils::timestampMs();
            this->id = id;
        }
    };

    class Buffer {
        private:
            QueueHandle_t xQueue;
        public:
            Buffer(int size) {
                xQueue = xQueueCreate(size, sizeof(Data *));
            }
            bool queue(Data* data) {
                return xQueueSend(xQueue, &data, portMAX_DELAY) == pdTRUE;
            }

            void clear() {
                xQueueReset(xQueue);
            }

            Data* dequeue() {
                Data* data;
                xQueueReceive(xQueue, &data, portMAX_DELAY);
                return data;
            }

            bool isEmpty() {
                return false; 
            }
    };

    class Parser {
        private: 
            data_type parsingType;
        public:
            Parser(data_type parsingType) {
                this->parsingType = parsingType;
            }
            /*format output: csv.*/
            csv parse(Data& data){
                if(data.id == this->parsingType) {
                    return doParse(data);
                } else {
                    return NULL;
                }
            }

        protected:
            virtual csv doParse(Data& data);
    };
}