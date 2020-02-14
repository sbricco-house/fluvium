#include "../task/Buffering.h"
#include "../network/Network.h"
#include "../middleware/MiddlewareService.h"
using namespace buffering;
using namespace middleware;

void launch_waterlevel(const Buffer & buffer, network::Network& net, middleware::MiddlewareService& service);

void launch_ground(const Buffer & buffer, network::Network& net, middleware::MiddlewareService& service);