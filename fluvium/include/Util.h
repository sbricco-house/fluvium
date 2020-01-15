#pragma once
#include <sys/time.h>
namespace timeutils {
    long timestampMs() {
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return tp.tv_sec * 1000 + tp.tv_usec / 1000;
    }

}