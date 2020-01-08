#include <iostream>
#include "scheduler/AtlasScheduler.h"
#include "coap/AtlasCoapServer.h"
#include "coap/AtlasCoapResource.h"
#include "coap/AtlasCoapMethod.h"
#include "coap/AtlasCoapResponse.h"
#include "logger/AtlasLogger.h"
#include "alarm/AtlasAlarm.h"

void test()
{
    std::cout << "timer expired" << std::endl;
}

int main(int argc, char **argv)
{
    atlas::initLog();

    atlas::AtlasCoapServer::getInstance().start("127.0.0.1", "10099", atlas::ATLAS_COAP_SERVER_MODE_BOTH, "12345678"); 
    
    atlas::AtlasAlarm alarm(1000, false, test);

    alarm.start();

    alarm.cancel();

    ATLAS_LOGGER_DEBUG("Starting Atlas gateway...");

    atlas::AtlasScheduler::getInstance().run();

    return 0;
};
