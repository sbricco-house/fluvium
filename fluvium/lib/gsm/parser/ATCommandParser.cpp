#include "ATCommandParser.h"

#include <cstring>

Response defaultResponseHandler(const char* response, int length) {
    if(strstr(response, MODEM_RESULT_CODE_OK)) {
        return Response::success("");
    }
    return Response::fail();
}

Response connectResponseHandler(const char* response, int length) {
    if(strstr(response, MODEM_RESULT_CODE_CONNECT)) {
        return Response::success("");
    }
    return Response::fail();
}

Response exitResponseHandler(const char* response, int length) {
    if(strstr(response, MODEM_RESULT_CODE_OK)) {
        return Response::success("");
    } else if (strstr(response, MODEM_RESULT_CODE_NOCARRIER)) {
        return Response::success("");
    }
    return Response::fail();
}

Response rssiReponseHandler(const char* response, int length) {
    if (!strncmp(response, "+CSQ", strlen("+CSQ"))) {
        printf("Parsing +csq\n");
        int rssi = 0;
        sscanf(response, "%d", &rssi);
        Response::success("" + rssi);
    }
    return Response::fail();
}
