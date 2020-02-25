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
#include "policy/AtlasPolicy.h"
#include "pubsub_agent/AtlasPubSubAgent.h"

int main(int argc, char **argv)
{
    atlas::AtlasRegister reg;
    atlas::AtlasPolicy policy;
    atlas::AtlasPubSubAgent pubSubAgent;
    
    atlas::initLog();

    atlas::AtlasCoapServer::getInstance().start(10100, atlas::ATLAS_COAP_SERVER_MODE_DTLS_PSK); 

    ATLAS_LOGGER_DEBUG("Starting Atlas gateway...");

    /* Start policy module */
    policy.start();

    /* Start registration module */
    reg.start();

    /* Start publish-subscribe agent */
    pubSubAgent.start();

    /* Start scheduler */
    atlas::AtlasScheduler::getInstance().run();

    /* Stop policy module */
    policy.stop();

    /* Stop registration module */
    reg.stop();

    ATLAS_LOGGER_DEBUG("Stopping Atlas gateway...");

    return 0;
};
