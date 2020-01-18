#pragma once

#include <sys/time.h>
#include <cmath>

namespace timeutils {
    static inline long timestampMs() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return tp.tv_sec * 1000 + tp.tv_usec / 1000;
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
};