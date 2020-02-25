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
#include "pubsub_agent/AtlasPubSubAgent.h"
#include "mqtt_client/AtlasMqttClient.h"


int main(int argc, char **argv)
{
    atlas::AtlasRegister reg;
    atlas::AtlasPubSubAgent pubSubAgent;
    
    if (argc == 2) {        
        atlas::AtlasMqttClient::getInstance().connect(argv[1]);
    } else {
        ATLAS_LOGGER_ERROR("Too few arguments used when atlas_gateway was executed!");
        std::cout << "Incorrect number of parameters." << std::endl << "Correct usage: atlas_gateway <cloud_hostname>" << std::endl;
        return 1;
    }

    
    // bool ans;
    // for (int i=0; i< 30; i++)
    // {
    //     ans = atlas::AtlasMqttClient::getInstance().tryPublishMessage("test1235", "mesaj de test" + std::to_string(i), 1);
    //     while (!ans)
    //     {
    //         ans = atlas::AtlasMqttClient::getInstance().tryPublishMessage("test1235", "mesaj de test" + std::to_string(i), 1);
    //         sleep(1.5);
    //     }   
    //     sleep(2);     
    // }

    atlas::initLog();

    atlas::AtlasCoapServer::getInstance().start(10100, atlas::ATLAS_COAP_SERVER_MODE_DTLS_PSK); 

    ATLAS_LOGGER_DEBUG("Starting Atlas gateway...");
    
    /* Start registration module */
    reg.start();

    /* Start publish-subscribe agent */
    pubSubAgent.start();

    /* Start scheduler */
    atlas::AtlasScheduler::getInstance().run();

    /* Stop registration module */
    reg.stop();

    ATLAS_LOGGER_DEBUG("Stopping Atlas gateway...");

    return 0;
};
