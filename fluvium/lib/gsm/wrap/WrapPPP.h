#pragma once

#include "freertos/FreeRTOS.h"
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"
#include "tcpip_adapter.h"

#ifdef __cplusplus
extern "C" {
    err_t ppp_input_to_tcpip(ppp_pcb *ppp, u8_t *data, int len);
    //err_t ppp_close(ppp_pcb* ppp, u8_t nocarrier);
}

#endif