#include <iostream>
#include "scheduler/AtlasScheduler.h"
#include "coap/AtlasCoapServer.h"
#include "coap/AtlasCoapResource.h"
#include "coap/AtlasCoapMethod.h"
#include "coap/AtlasCoapResponse.h"
#include "logger/AtlasLogger.h"
#include "alarm/AtlasAlarm.h"
#include "coap/AtlasCoapClient.h"
#include "register/AtlasRegister.h"

int main(int argc, char **argv)
{
    atlas::AtlasRegister reg;
    
    atlas::initLog();

    atlas::AtlasCoapServer::getInstance().start("127.0.0.1", "10099", atlas::ATLAS_COAP_SERVER_MODE_BOTH); 

    ATLAS_LOGGER_DEBUG("Starting Atlas gateway...");
    
    /* Start registration module */
    reg.start();

    /* Start scheduler */
    atlas::AtlasScheduler::getInstance().run();

    /* Stop registration module */
    reg.stop();

    ATLAS_LOGGER_DEBUG("Stopping Atlas gateway...");

    return 0;
};
