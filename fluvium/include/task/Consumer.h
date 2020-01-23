#pragma once
#include "Task.h"
#include "middleware/MiddlewareService.h"
#include "network/Network.h"
#include "Buffering.h"
using namespace buffering;
namespace task {
    struct ParserSet {
        Parser * elements;
        int len;
    };
    class Consumer : public Task {
        private:
            middleware::MiddlewareService& middleware;
            network::Network& net;
            ParserSet& parsers;
        public:
            Consumer(const Buffer& buffer, middleware::MiddlewareService& middleware, network::Network& net, ParserSet& parsers);
            void run() override;
    };
}