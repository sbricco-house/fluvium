#pragma once 
#include "MiddlewareService.h"
namespace middleware {
    class FakeMiddleware : public MiddlewareService {
        public:
            FakeMiddleware();
            ConnectionResult connect() override;
            ConnectionResult reconnect() override;
            ConnectionResult disconnect() override;
            PublishResult publish(char * payload) override;
    };
}