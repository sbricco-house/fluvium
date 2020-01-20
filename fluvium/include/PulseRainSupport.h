#include "RainGauge.h"
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "Util.h"
#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
namespace support {
    //TODO create a support to manage interrupts
    #define ESP_INTR_FLAG_DEFAULT 0
    class PulseRain : public device::RainGauge {
        #define BOUNDING_THR 300
        private:
            
            portMUX_TYPE mutex;
            metric::millimeter rainQuantity;
            timeutils::millisecond lastSensed;
            gpio_num_t pulsablePin;
            metric::millimeter delta;
            static void PULSE_REC(void* arg) {
                timeutils::millisecond current = timeutils::timestampMs();
                PulseRain* self = (PulseRain*)(arg);
                portENTER_CRITICAL_ISR(&self->mutex);
                if(current - self->lastSensed < BOUNDING_THR) {
                    return;
                }
                self->lastSensed = current;
                self->rainQuantity += self->delta;
                portEXIT_CRITICAL_ISR(&self->mutex);
            }
            SemaphoreHandle_t semaphore;
        public:
            PulseRain(gpio_num_t pulsablePin, metric::millimeter deltaQuantity) 
            : pulsablePin(pulsablePin),
            delta(deltaQuantity) {
                semaphore = xSemaphoreCreateBinary();
                xSemaphoreGive(semaphore);
                lastSensed = timeutils::timestampMs();
                mutex = portMUX_INITIALIZER_UNLOCKED;
            }
            void init() override {
                gpio_pad_select_gpio(pulsablePin);
	            gpio_set_direction(pulsablePin, GPIO_MODE_INPUT);
                gpio_pullup_en(pulsablePin);
                gpio_set_intr_type(pulsablePin, GPIO_INTR_POSEDGE);
                gpio_isr_handler_add(pulsablePin, PULSE_REC, this);
            }
            metric::millimeter getRainQuantity() override {
                portENTER_CRITICAL(&mutex);
                float returnValue = rainQuantity;
                portEXIT_CRITICAL(&mutex);
                return returnValue;
            }
            void reset() override {
                portENTER_CRITICAL(&mutex);
                rainQuantity = 0;
                portEXIT_CRITICAL(&mutex);
            }
            metric::millimeter getAndResetQuantity() override {
                portENTER_CRITICAL(&mutex);
                float returnValue = rainQuantity;
                rainQuantity = 0;
                portEXIT_CRITICAL(&mutex);
                return returnValue;
            }      
    };
}