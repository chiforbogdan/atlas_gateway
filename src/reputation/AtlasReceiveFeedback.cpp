#include <string>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include "AtlasReceiveFeedback.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"
#include "../mosquitto_plugin/filter/AtlasFilter.h"

namespace atlas {

const std::string ATLAS_FEEDBACK_RECEIVE_URI = "gateway/reputation/feedback";


AtlasReceiveFeedback::AtlasReceiveFeedback() : receiveFeedbackResource_(ATLAS_FEEDBACK_RECEIVE_URI,
                                                ATLAS_COAP_METHOD_PUT,
                                                boost::bind(&AtlasReceiveFeedback::receiveFeedbackCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)){}

AtlasCoapResponse AtlasReceiveFeedback::receiveFeedbackCallback(const std::string &path, const std::string &pskIdentity,
                                                                const std::string &psk, AtlasCoapMethod method,
                                                                const uint8_t* reqPayload, size_t reqPayloadLen,
                                                                uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    AtlasCommandBatch cmdRespBatch;
    std::vector<AtlasCommand> cmd;
    std::pair<const uint8_t*, size_t> cmdBuf;
    std::string identity;
    uint16_t feedback;
   
    
    ATLAS_LOGGER_DEBUG("Feedback callback executed...");

    ATLAS_LOGGER_INFO1("Process FEEDBACK command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice& device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if (!device.isRegistered()) {
        ATLAS_LOGGER_ERROR("Received FEEDBACK command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse command */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Feedback end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Feedback end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Feedback end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } 
        else if(cmdEntry.getType() == ATLAS_CMD_FEEDBACK){
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Feedback end-point called with empty FEEDBACK command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            memcpy(&feedback, cmdEntry.getVal(), cmdEntry.getLen());
        }
    }

    if (identity == "") {
        ATLAS_LOGGER_ERROR("Receive feedback failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /*if (!feedback) {
        ATLAS_LOGGER_ERROR("Receive feedback failed because of invalid feedback");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }*/


    ATLAS_LOGGER_INFO("Device with identity " + identity + " send a feedback value.");
    
 
    return ATLAS_COAP_RESP_OK;
}

void AtlasReceiveFeedback::start()
{
    ATLAS_LOGGER_DEBUG("Start FEEDBACK Reputation module");

    /* Add CoAP resource for receive feedback */
    AtlasCoapServer::getInstance().addResource(receiveFeedbackResource_);
}

void AtlasReceiveFeedback::stop()
{
    ATLAS_LOGGER_DEBUG("Stop FEEDBACK reputation module");

    /* Remove CoAP resource for receive feedback */
    AtlasCoapServer::getInstance().delResource(receiveFeedbackResource_);
}

} // namespace atlas
