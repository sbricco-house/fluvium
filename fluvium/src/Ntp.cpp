#include "Ntp.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/apps/sntp.h"

namespace ntputils {
    const char* TAG = "ntp";
    char* NTP_SERVER1 = "pool.ntp.org";
}

// internal functions
static void initializeNtp();
static void waitNtpUpdate();

void ntputils::setupTime() {
    initializeNtp();
    waitNtpUpdate();
    sntp_stop();
}

static void initializeNtp() {
    ESP_LOGI(ntputils::TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, ntputils::NTP_SERVER1);
    sntp_init();
}

static void waitNtpUpdate() {
    time_t now;
    struct tm timeinfo;
    do {
        time(&now);
        localtime_r(&now, &timeinfo);
        vTaskDelay(pdMS_TO_TICKS(500));
    } while(timeinfo.tm_year <= 70);
}