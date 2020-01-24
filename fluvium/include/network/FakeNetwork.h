#pragma once
#include "Network.h"
namespace network {
    class FakeNetwork : public Network {
        public:
            FakeNetwork();
            NetworkState getState() override;
            bool connect() override;
            bool disconnect() override;
            bool standby() override;
        private:
    };
}