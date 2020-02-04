#include "support/SonarSupport.h"
#include "esp_log.h"
using namespace support;
double Sonar::computeSoundVelocity(metric::celsius temp) {
    return SOUND_VEL + (TEMP_RELATION * temp);
}
void Sonar::SONAR_ECHO(void* arg) {
    Sonar * self = (Sonar *) arg;
    self->deltaTime = timeutils::usToS(timeutils::timestampUs() - self->currentTime);
    BaseType_t priority = pdFALSE;
    xEventGroupSetBitsFromISR(self->group,BIT0, &priority);
}
Sonar::Sonar(gpio_num_t trigPin, gpio_num_t echoPin, metric::meter maxDistance, timeutils::millisecond maxTimeout)
    : trigPin(trigPin), echoPin(echoPin), maxDistance(maxDistance) {
    timeoutMeasurement = pdMS_TO_TICKS(maxTimeout); //TODO put in timeutils
    group = xEventGroupCreate();
    deltaTime = 0;
}
void Sonar::init() {
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

metric::meter Sonar::senseDistance(metric::celsius temperature) {
    printf("temp = %f\n", temperature);
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
    printf("DELTA = %f\n", deltaTime);
    return distance > maxDistance ? maxDistance : distance;
}