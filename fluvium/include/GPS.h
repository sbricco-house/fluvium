#pragma once

#include "Buffering.h"

namespace buffering {
    #define gpsType 2
    namespace data {
        struct Location : public Data {
            // TODO: to be defined
        };
    }

    namespace parser {
        class LocationParser : public Parser {
            public:
                LocationParser() : Parser(gpsType) {};
            protected:
                csv doParse(Data& data) {
                    return ""; // TODO: define parsing
                }
        };
    }
}
