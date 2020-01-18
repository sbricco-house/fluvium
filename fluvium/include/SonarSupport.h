#pragma once
#include "DistanceSensor.h"
#include "Metric.h"
#include "Util.h"
#include <driver/gpio.h>
#include <freertos/event_groups.h>
#include <freertos/FreeRTOS.h>
using namespace device;
namespace support {
    class Sonar : public DistanceSensor {
        #define RESET_TIME 2
        #define TRIGGER_SIGNAL_TIME 10
        #define SOUND_VEL 331.3
        #define TEMP_RELATION 0.606
        #define ESP_INTR_FLAG_DEFAULT 0
        private:
            gpio_num_t trigPin;
            gpio_num_t echoPin;
            metric::meter maxDistance;
            timeutils::microsecond currentTime;
            float timeoutMeasurement;
            timeutils::second deltaTime = 0;
            EventGroupHandle_t group = xEventGroupCreate(); //used to synchronize echo phases.
            double computeSoundVelocity(metric::celsius temp) {
                return SOUND_VEL + (TEMP_RELATION * temp);
            }
            //ISR ROUTINE
            static void SONAR_ECHO(void* arg) {
                Sonar * self = (Sonar *) arg;
                self->deltaTime = timeutils::usToS(timeutils::timestampUs() - self->currentTime);
                BaseType_t priority = pdFALSE;
                xEventGroupSetBitsFromISR(self->group,BIT0, &priority);
            }
        public:
            Sonar(gpio_num_t trigPin,
                gpio_num_t echoPin,
                metric::meter maxDistance = 6.0,
                timeutils::millisecond maxTimeout = 120) :
                trigPin(trigPin),
                echoPin(echoPin),
                maxDistance(maxDistance) {
                    timeoutMeasurement = pdMS_TO_TICKS(maxTimeout); //TODO put in timeutils
                }

            void init() override {
                gpio_reset_pin(echoPin);
                //configure each pin
                gpio_pad_select_gpio(trigPin);
	            gpio_pad_select_gpio(echoPin);
                //set pin direction
                gpio_set_direction(trigPin, GPIO_MODE_OUTPUT);
                gpio_set_direction(echoPin, GPIO_MODE_INPUT);
                //install isr service on echo                
                gpio_set_intr_type(echoPin, GPIO_INTR_NEGEDGE);
                gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT); //TODO VERIFY HERR 
            }  
            metric::meter senseDistance(metric::celsius temperature) override {
                //reset old state
                gpio_set_level(trigPin, LOW);
                ets_delay_us(RESET_TIME);
                //trigger signal
                gpio_set_level(trigPin, HIGH);
                ets_delay_us(TRIGGER_SIGNAL_TIME);
                gpio_set_level(trigPin, LOW);
                currentTime = timeutils::timestampUs();
                gpio_isr_handler_add(echoPin, SONAR_ECHO, this);
                xEventGroupWaitBits(group, BIT0, pdTRUE, pdTRUE, timeoutMeasurement);
                gpio_isr_handler_remove(echoPin);
                float distance = computeSoundVelocity(temperature) * (deltaTime / 2.0);
                return distance > maxDistance ? maxDistance : distance;
            }
    };
}