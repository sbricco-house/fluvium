#pragma once

#include "DCE.h"

#define MODEM_RESULT_CODE_OK    "OK"
#define MODEM_RESULT_CODE_ERROR "ERROR"
#define MODEM_RESULT_CODE_CONNECT "CONNECT"
#define MODEM_RESULT_CODE_NOCARRIER "NO CARRIER"

using namespace dce;

Response defaultResponseHandler(const char* response, int length);
Response connectResponseHandler(const char* response, int length);
Response exitResponseHandler(const char* response, int length);
Response rssiReponseHandler(const char* response, int length);