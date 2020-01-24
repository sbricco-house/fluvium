#include "network/FakeNetwork.h"
using namespace network;
FakeNetwork::FakeNetwork() {}

NetworkState FakeNetwork::getState() {
    return CONNECTED;
}

bool FakeNetwork::connect() {
    return true;
}
bool FakeNetwork::disconnect() {
    return true;
}
bool FakeNetwork::standby() {
    return true;
}
