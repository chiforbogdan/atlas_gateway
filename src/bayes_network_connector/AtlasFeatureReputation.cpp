#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include "AtlasFeatureReputation.h"
#include "AtlasCoapPush.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"
#include "../mosquitto_plugin/filter/AtlasFilter.h"

namespace atlas {

const std::string ATLAS_FEATURE_RECEIVE_URI = "gateway/feature";


AtlasFeatureReputation::AtlasFeatureReputation() : featureReputationResource_(ATLAS_FEATURE_RECEIVE_URI,
                                                    ATLAS_COAP_METHOD_PUT,
                                                    boost::bind(&AtlasFeatureReputation::featureReputationCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)){}


AtlasCoapResponse AtlasFeatureReputation::featureReputationCallback(const std::string &path, const std::string &pskIdentity,
                                                  const std::string &psk, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    
    ATLAS_LOGGER_DEBUG("Feature callback executed...");

    ATLAS_LOGGER_INFO1("Process FEATURE command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice& device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if (!device.isRegistered()) {
        ATLAS_LOGGER_ERROR("Received FEATURE command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse command */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Feature end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Feature end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Feature end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } 
        else if(cmdEntry.getType() == ATLAS_CMD_FEATURE_REQUEST){
            feature.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            std::cout << "Sending random value for feature " << feature << std::endl;
            sendReputationValue();
        }
    }

    if (identity == "") {
        ATLAS_LOGGER_ERROR("Feature failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    
    return ATLAS_COAP_RESP_OK;
}

void AtlasFeatureReputation::start()
{
    ATLAS_LOGGER_DEBUG("Start FEATURE module");

    /* Add CoAP resource for reputation feature */
    AtlasCoapServer::getInstance().addResource(featureReputationResource_);
}

void AtlasFeatureReputation::sendReputationValue()
{
    AtlasCoapPush *at = new AtlasCoapPush(identity);
    at->push();
}

}