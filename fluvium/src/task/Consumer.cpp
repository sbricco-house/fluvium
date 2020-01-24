#include "task/Consumer.h"
#include "Util.h"
#include "esp_log.h"
using namespace task;
using namespace buffering;
using namespace middleware;
using namespace network;

Consumer::Consumer(const Buffer& buffer, MiddlewareService& middleware, Network& net, ParserSet parsers) : Task(buffer), middleware(middleware), net(net), parsers(parsers) {}
//TODO add controls on data publish
void Consumer::run() {
    net.connect();
    middleware.connect();
    while(!buffer.isEmpty()) {
        auto data = buffer.dequeue();
        json payload = "";
        for(int i = 0; i < parsers.len ; i++) {
            payload = parsers.elements[i]->serialize(*data);
            if(payload != "") {
                auto bufferSize = payload.length() + 1;
                char payloadBuffer[bufferSize];
                strcpy(payloadBuffer, payload.c_str());
                ESP_LOGI("aws_publisher", "publishing = %s", payloadBuffer);
                middleware.publish(payloadBuffer);
            }
        }
        delete data;
    }
    net.disconnect();
    middleware.disconnect();    
}