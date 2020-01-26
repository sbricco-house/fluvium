#pragma once

namespace network {
    enum NetworkState {
        CONNECTING,
        CONNECTED,
        DISCONNECTED,
        STANDBY
    };
    class Network {
        public:
            virtual NetworkState getState() = 0;
            virtual bool connect() = 0;
            virtual bool disconnect() = 0;
            virtual bool standby() = 0;
    };
}