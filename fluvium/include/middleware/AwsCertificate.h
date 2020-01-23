#include <freertos/FreeRTOS.h>

extern const uint8_t aws_root_ca_pem_start[] asm("_binary_cert_aws_root_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_cert_aws_root_pem_end");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_cert_certificate_pem_crt_start");
extern const uint8_t certificate_pem_crt_end[] asm("_binary_cert_certificate_pem_crt_end");
extern const uint8_t private_pem_key_start[] asm("_binary_cert_private_pem_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_cert_private_pem_key_end");