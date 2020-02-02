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
            bool connectWithAttempts(int connectionAttempts = 0) {
                bool connected = false;
                for(int i = -1; // in case of single attempt
                    i < connectionAttempts && !connected;
                    i++, connected = connect()) {}
                return connected;
            }
    };
}