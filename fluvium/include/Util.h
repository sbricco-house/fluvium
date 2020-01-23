#pragma once

#include <sys/time.h>
#include <time.h>
#include <str.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cmath>
#include "nmea_parser.h"

namespace timeutils {
    using microsecond = long long;
    using millisecond = uint64_t;
    using second = double;

    static inline second usToS(microsecond us) {
        return us / 1000000.0;
    }
    static inline millisecond timestampMs() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return tp.tv_sec * 1000 + tp.tv_usec / 1000;
    }
    static inline microsecond timestampUs() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return tp.tv_sec * 1000000L + tp.tv_usec;
    }
};

namespace stringutils {
    static inline bool startWith(const char* string, const char* start) {
        bool startWith = true;
        for(int i = 0; start[i] != '\0' && startWith; i++) {
            startWith &= string[i] == start[i];
        }
        return startWith;
    }

    static inline void skipCharacters(const char** string, int skipLength) {
        (*string) += skipLength;
    }

    static inline int hex2int(char c)
    {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        return -1;
    };

    static inline bool isEmpty(char * word) {
        return strcmp(word, "")  == 0;
    }
};

namespace gpsutils {
    static inline double degreesMinutesToDegree(double degrees, double minutes) {
        return degrees + (minutes / 60);
    }

    static inline double degreesMinutesToDegree(double degreesMinutes) {
        double degree = (int) floor(degreesMinutes / 100);
        double minutes = degreesMinutes - (degree * 100);
        return degreesMinutesToDegree(degree, minutes);
    }

    static inline timeutils::millisecond dateTimeToTimestamp(gps_date_t date, gps_time_t time) {
        struct tm t;
        t.tm_year = (2000 + date.year) - 1900;
        t.tm_mon = date.month - 1;
        t.tm_mday = date.day;
        t.tm_hour = time.hour;
        t.tm_min = time.minute;
        t.tm_sec = time.second;
        return ((uint64_t) mktime(&t)) * 1000;
    }
};
