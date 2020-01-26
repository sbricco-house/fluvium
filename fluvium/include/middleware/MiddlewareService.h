#pragma once
namespace middleware {
    enum ConnectionResult {
        OK = 0,
        NO_CONNECTION = -1,
        INITIALIZATION_PROBLEM = -2,
        DISCONNECTION_PROBLEM = -3
    };

    enum PublishResult {
        PUBLISHED = 0,
        PUBLISH_ERROR = -1
    };

    class MiddlewareService {
        public:
            virtual ConnectionResult connect() = 0;
            virtual PublishResult publish(char * payload) = 0;
            virtual ConnectionResult disconnect() = 0;
    };
}