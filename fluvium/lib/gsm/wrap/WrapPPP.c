#include "WrapPPP.h"

err_t ppp_input_to_tcpip(ppp_pcb *ppp, u8_t *data, int len) {
    return pppos_input_tcpip(ppp, data, len);
}
/*
err_t ppp_close(ppp_pcb* ppp, u8_t nocarrier) {
    return pppapi_close(ppp, nocarrier);
}*/