#include "support/PulseRainSupport.h"
using namespace support;
#define BOUNDING_THR 300
        
void PulseRain::PULSE_REC(void* arg) {
    PulseRain* self = (PulseRain*)(arg);
    portENTER_CRITICAL_ISR(&self->mutex);
    timeutils::millisecond current = timeutils::timestampMs();
    if(current - self->lastSensed < BOUNDING_THR) {
        portEXIT_CRITICAL_ISR(&self->mutex);
        return;
    }
    self->lastSensed = current;
    self->rainQuantity += self->delta;
    portEXIT_CRITICAL_ISR(&self->mutex);
}
PulseRain::PulseRain(gpio_num_t pulsablePin, metric::millimeter deltaQuantity) : pulsablePin(pulsablePin), delta(deltaQuantity) {
    lastSensed = timeutils::timestampMs();
    mutex = portMUX_INITIALIZER_UNLOCKED;
    rainQuantity = 0;
}

void PulseRain::init() {
    gpio_pad_select_gpio(pulsablePin);
    gpio_set_direction(pulsablePin, GPIO_MODE_INPUT);
    gpio_pullup_en(pulsablePin);
    gpio_set_intr_type(pulsablePin, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0); //TODO FIND BEST PLACE TO PUT THIS:.
    gpio_isr_handler_add(pulsablePin, PULSE_REC, this);
}
metric::millimeter PulseRain::getRainQuantity() {
    portENTER_CRITICAL(&mutex);
    float returnValue = rainQuantity;
    portEXIT_CRITICAL(&mutex);
    printf("Rain quantity: %lf\n", returnValue);
    return returnValue;
}
void PulseRain::reset() {
    portENTER_CRITICAL(&mutex);
    rainQuantity = 0;
    portEXIT_CRITICAL(&mutex);
}
metric::millimeter PulseRain::getAndResetQuantity() {
    portENTER_CRITICAL(&mutex);
    float returnValue = rainQuantity;
    rainQuantity = 0;
    portEXIT_CRITICAL(&mutex);
    return returnValue;
}