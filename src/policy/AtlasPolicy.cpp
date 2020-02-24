#include <string>
#include <boost/bind.hpp>
#include "AtlasPolicy.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"
#include "../mosquitto_plugin/filter/AtlasFilter.h"
#include "../pubsub_agent/AtlasPubSubAgent.h"

namespace atlas {

const std::string ATLAS_FIREWALLPOLICY_URI = "gateway/policy";

AtlasPolicy::AtlasPolicy() : firewallPolicyResource_(ATLAS_FIREWALLPOLICY_URI,
                                                    ATLAS_COAP_METHOD_POST,
                                                    boost::bind(&AtlasPolicy::firewallPolicyCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)){}


AtlasCoapResponse AtlasPolicy::firewallPolicyCallback(const std::string &path, const std::string &pskIdentity,
                                                  const std::string &psk, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    atlas::AtlasPubSubAgent pubSubAgent;
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string clientId = "";

    uint16_t *qos = nullptr;
    uint16_t *ppm = nullptr;
    uint16_t *payloadLen = nullptr;
    
    ATLAS_LOGGER_DEBUG("Firewall policy callback executed...");

    ATLAS_LOGGER_INFO1("Process FIREWALL POLICY INSTALL command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice& device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if (!device.isRegistered()) {
        ATLAS_LOGGER_ERROR("Received FIREWALL POLICY INSTALL command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Firewall policy end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Start publish-subscribe agent */
    pubSubAgent.start();

    for (AtlasCommand &cmdEntry : cmd) 
    {
        if (cmdEntry.getType() == ATLAS_CMD_DATA_PLANE_POLICY_CLIENTID) 
        {
            ATLAS_LOGGER_DEBUG("Policy end-point called and CLIENT ID command is found");
           
            if (!cmdEntry.getLen()) 
            {
                ATLAS_LOGGER_ERROR("Policy end-point called with empty CLIENT ID command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            clientId.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
        }
        else if (cmdEntry.getType() == ATLAS_CMD_DATA_PLANE_POLICY_MAX_QOS) 
        {
            ATLAS_LOGGER_DEBUG("Policy end-point called and QOS command is found");
           
            if (!cmdEntry.getLen()) 
            {
                ATLAS_LOGGER_ERROR("Policy end-point called with empty QOS command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            qos = new uint16_t[cmdEntry.getLen()];
            memcpy(qos, cmdEntry.getVal(), cmdEntry.getLen());
        }
        else if (cmdEntry.getType() == ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_PER_MINUTE) 
        {
            ATLAS_LOGGER_DEBUG("Policy end-point called and PPM command is found");
           
            if (!cmdEntry.getLen()) 
            {
                ATLAS_LOGGER_ERROR("Policy end-point called with empty PPM command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            ppm = new uint16_t[cmdEntry.getLen()];
            memcpy(ppm, cmdEntry.getVal(), cmdEntry.getLen());
        }
        else if (cmdEntry.getType() == ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_MAXLEN) 
        {
            ATLAS_LOGGER_DEBUG("Policy end-point called and PAYLOAD_LEN command is found");
           
            if (!cmdEntry.getLen()) 
            {
                ATLAS_LOGGER_ERROR("Policy end-point called with empty PAYLOAD_LEN command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            payloadLen = new uint16_t[cmdEntry.getLen()];
            memcpy(payloadLen, cmdEntry.getVal(), cmdEntry.getLen());
        }
    }

    if (clientId == "" || !qos || !ppm || !payloadLen) 
    {
        ATLAS_LOGGER_ERROR("Policy failed because of invalid params");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Install firewall policy in mosquitto plug-in*/
    pubSubAgent.installFirewallRule(clientId, *qos, *ppm, *payloadLen);

    return ATLAS_COAP_RESP_OK;
}


void AtlasPolicy::start()
{

    ATLAS_LOGGER_DEBUG("Start policy module");

    AtlasCoapServer::getInstance().addResource(firewallPolicyResource_);
}

void AtlasPolicy::stop()
{
    ATLAS_LOGGER_DEBUG("Stop policy module");
    
    AtlasCoapServer::getInstance().delResource(firewallPolicyResource_);
}

} // namespace atlas
