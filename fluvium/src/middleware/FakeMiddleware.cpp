#include "middleware/FakeMiddleware.h"
using namespace middleware;

FakeMiddleware::FakeMiddleware() {}
ConnectionResult FakeMiddleware::connect() {
    return OK;
}
ConnectionResult FakeMiddleware::disconnect() {
    return OK;
}
PublishResult FakeMiddleware::publish(char * payload) {
    return PUBLISHED;
}