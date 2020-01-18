#pragma once
#include "Buffering.h"
#include "Task.h"
#include <cstdio>

namespace buffering {
    #define waterLevelType 1

    namespace data {
        using millimeter = uint16_t; //max value: 65 m
        struct WaterLevel : public Data {
            millimeter level;
            WaterLevel(millimeter level) : Data(waterLevelType) {
                this->level = level;
            }
        };
    }
    namespace parser {
        class WaterLevelParser : public Parser {
            #define parser_buffer_size 20 // <uint_16.charlength>;<long.charlength>;
            public:
                WaterLevelParser() : Parser(waterLevelType) {}
            protected: 
                csv doSerialize(const Data& data) override {
                    data::WaterLevel& level = *(data::WaterLevel*)(&data);
                    char buffer[parser_buffer_size];
                    sprintf(buffer, "%d;%ld", level.level, level.timestap);
                    return csv(buffer);
                }
        };
    }
}
namespace task {
    class WaterLevelTask : public Task {
        public:
            void run() override {

            }
    };
}