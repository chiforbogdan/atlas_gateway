#include <iostream>
#include <boost/program_options.hpp>
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
#include "reputation/AtlasFeatureReputation.h"
#include "cloud/AtlasCloudCmdParser.h"
#include "sql/AtlasSQLite.h"

namespace {

const std::string ATLAS_GATEWAY_DESC = "Atlas gateway";
const int ATLAS_COAP_MAX_PORT = 65535;

/* Cloud back-end hostname*/
std::string cloudHostname;

/* CoAP listen port for the client connection */
int coapPort;

} // anonymous namespace

void parse_options(int argc, char **argv)
{
    boost::program_options::options_description desc(ATLAS_GATEWAY_DESC);
    boost::program_options::variables_map vm;
    
    desc.add_options()
    ("help", "Display help message")
    ("cloud,c", boost::program_options::value<std::string>(&cloudHostname), "IP address for the cloud broker - used to connect the gateway with the cloud back-end")
    ("listen,l", boost::program_options::value<int>(&coapPort), "Listen port number for the gateway CoAP server - used to connect the gateway with the client");


    try {

        boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
                                      .options(desc).run(), vm);
        
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(1);
        }

        boost::program_options::notify(vm);
    
        /* Port validation */
        if (coapPort <= 0 || coapPort > ATLAS_COAP_MAX_PORT) {
            std::cout << "ERROR: Invalid listening port" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        }

        /* Cloud hostname validation */
        if (cloudHostname == "") {
            std::cout << "ERROR: Invalid cloud back-end hostname" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        }

    } catch(boost::program_options::error& e) {
        std::cout << desc << std::endl;
        exit(1);
    }
}

int main(int argc, char **argv)
{
    atlas::AtlasRegister reg;
    atlas::AtlasPolicy policy;
    atlas::AtlasFeatureReputation reputation;

    parse_options(argc, argv);

    atlas::initLog();
    
    ATLAS_LOGGER_DEBUG("Starting Atlas gateway...");

    if(!atlas::AtlasIdentity::getInstance().initIdentity()) {
        ATLAS_LOGGER_ERROR("Error in generating gateway identity!");
        return 1;
    }

    /* Connect to cloud back-end */
    atlas::AtlasMqttClient::getInstance().connect(cloudHostname,
                                                  atlas::AtlasIdentity::getInstance().getIdentity());

    /* Start cloud register module */
    atlas::AtlasRegisterCloud::getInstance().start();

    /* Start cloud command parser module*/
    atlas::AtlasCloudCmdParser::getInstance().start();

    /*open local.db connection*/
    atlas::AtlasSQLite::getInstance().openConnection(atlas::ATLAS_DB_PATH);

    /* Start internal CoAP server */
    atlas::AtlasCoapServer::getInstance().start(coapPort, atlas::ATLAS_COAP_SERVER_MODE_DTLS_PSK); 

    /* Start policy module */
    policy.start();

    /* Start registration module */
    reg.start();

    /*Start feature reputation request */
    reputation.start();

    /* Start publish-subscribe agent */
    atlas::AtlasPubSubAgent::getInstance().start();

    /* Start scheduler */
    atlas::AtlasScheduler::getInstance().run();

    /* Stop cloud command parser module*/
    atlas::AtlasCloudCmdParser::getInstance().stop();

    /* Stop cloud register module */
    atlas::AtlasRegisterCloud::getInstance().stop();

    /* Stop feature reputation module */
    reputation.stop();

    /* Stop policy module */
    policy.stop();

    /* Stop registration module */
    reg.stop();

    /*close local.db connection*/
    atlas::AtlasSQLite::getInstance().closeConnection();

    ATLAS_LOGGER_DEBUG("Stopping Atlas gateway...");

    return 0;
};
