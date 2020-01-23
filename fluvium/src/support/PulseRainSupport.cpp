#include "support/PulseRainSupport.h"
using namespace support;
void PulseRain::PULSE_REC(void* arg) {
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
PulseRain::PulseRain(gpio_num_t pulsablePin, metric::millimeter deltaQuantity) : pulsablePin(pulsablePin), delta(deltaQuantity) {
    semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore);
    lastSensed = timeutils::timestampMs();
    mutex = portMUX_INITIALIZER_UNLOCKED;
}

void PulseRain::init() {
    gpio_pad_select_gpio(pulsablePin);
    gpio_set_direction(pulsablePin, GPIO_MODE_INPUT);
    gpio_pullup_en(pulsablePin);
    gpio_set_intr_type(pulsablePin, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(pulsablePin, PULSE_REC, this);
}
metric::millimeter PulseRain::getRainQuantity() {
    portENTER_CRITICAL(&mutex);
    float returnValue = rainQuantity;
    portEXIT_CRITICAL(&mutex);
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