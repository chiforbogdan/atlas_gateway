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
#include "mqtt_client/AtlasMqttClient.h"
#include "identity/AtlasIdentity.h"
#include "cloud/AtlasRegisterCloud.h"
#include "bayes_network_connector/AtlasFeatureReputation.h"

int main(int argc, char **argv)
{
    atlas::AtlasRegister reg;
    atlas::AtlasPolicy policy;
    atlas::AtlasFeatureReputation feature;

    atlas::initLog();
    
    if(!atlas::AtlasIdentity::getInstance().initIdentity()) {
        ATLAS_LOGGER_ERROR("Error in generating gateway identity!");
        return 1;
    }

    ATLAS_LOGGER_DEBUG("Identity generated with success!");

    if (argc == 2) {        
        atlas::AtlasMqttClient::getInstance().connect(argv[1]);
    } else {
        ATLAS_LOGGER_ERROR("Too few arguments used when atlas_gateway was executed!");
        std::cout << "Incorrect number of parameters." << std::endl << "Correct usage: atlas_gateway <cloud_hostname>" << std::endl;
        return 1;
    }

    /* Start cloud register module */
    atlas::AtlasRegisterCloud::getInstance().start();

    atlas::AtlasCoapServer::getInstance().start(10100, atlas::ATLAS_COAP_SERVER_MODE_DTLS_PSK); 

    ATLAS_LOGGER_DEBUG("Starting Atlas gateway...");

    /* Start policy module */
    policy.start();
    
    /* Start registration module */
    reg.start();

    /*Start feature value request */
    feature.start();

    /* Start publish-subscribe agent */
    atlas::AtlasPubSubAgent::getInstance().start();

    /* Start scheduler */
    atlas::AtlasScheduler::getInstance().run();

    /* Stop cloud register module */
    atlas::AtlasRegisterCloud::getInstance().stop();


    /* Stop policy module */
    policy.stop();

    /* Stop registration module */
    reg.stop();

    ATLAS_LOGGER_DEBUG("Stopping Atlas gateway...");

    return 0;
};
