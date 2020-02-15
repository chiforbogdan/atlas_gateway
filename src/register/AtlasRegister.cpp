#include <string>
#include <boost/bind.hpp>
#include "AtlasRegister.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

const std::string ATLAS_REGISTER_URI = "gateway/register";
const std::string ATLAS_KEEPALIVE_URI = "gateway/keepalive";

AtlasRegister::AtlasRegister() : registerResource_(ATLAS_REGISTER_URI,
                                                   ATLAS_COAP_METHOD_POST,
                                                   boost::bind(&AtlasRegister::registerCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                 keepAliveResource_(ATLAS_KEEPALIVE_URI,
                                                    ATLAS_COAP_METHOD_PUT,
                                                    boost::bind(&AtlasRegister::keepaliveCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)) {}

AtlasCoapResponse AtlasRegister::keepaliveCallback(const std::string &path, const std::string &pskIdentity,
                                                   const std::string& psk, AtlasCoapMethod method,
                                                   const uint8_t* reqPayload, size_t reqPayloadLen,
                                                   uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string identity = "";
    uint8_t *token = nullptr;
    
    ATLAS_LOGGER_DEBUG("Keepalive callback executed...");

    ATLAS_LOGGER_INFO1("Process KEEPALIVE command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice& device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if (!device.isRegistered()) {
        ATLAS_LOGGER_ERROR("Received KEEPALIVE command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    
    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Keep-alive end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_KEEPALIVE) {
            ATLAS_LOGGER_DEBUG("Keep-alive end-point called and KEEPALIVE command is found");
           
            /* The keep-alive token must have two bytes length */
            if (cmdEntry.getLen() != sizeof(uint16_t)) {
                ATLAS_LOGGER_ERROR("Keep-alive end-point called with invalid KEEPALIVE token length");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            token = new uint8_t[cmdEntry.getLen()];
            memcpy(token, cmdEntry.getVal(), cmdEntry.getLen());
        } else if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            ATLAS_LOGGER_DEBUG("Keep-alive end-point called and IDENTITY command is found");
           
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Keep-alive end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Keep-alive end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        }

    }

    if (identity == "" || !token) {
        ATLAS_LOGGER_ERROR("Keep-alive failed because of invalid identity or keep-alive token");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Echo keep-alive token */
    *respPayload = token;
    *respPayloadLen = sizeof(uint16_t);

    ATLAS_LOGGER_INFO1("Keep-alive SUCCESS sent by client with identity ", identity);

    /* Notify device that a keep-alive was just received */
    device.keepAliveNow();
    
    return ATLAS_COAP_RESP_OK;
}

AtlasCoapResponse AtlasRegister::registerCallback(const std::string &path, const std::string &pskIdentity,
                                                  const std::string &psk, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string identity;

    ATLAS_LOGGER_DEBUG("Register callback executed...");

    ATLAS_LOGGER_INFO1("Process REGISTER command from client with DTLS PSK identity ", pskIdentity);

    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Registration end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_REGISTER) {
            ATLAS_LOGGER_DEBUG("Registration end-point called and REGISTER command is found");
           
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Registration end-point called with empty REGISTER command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Register end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
 
            ATLAS_LOGGER_INFO1("New ATLAS client registered with identity ", identity);

            /* Save the device registration timestamp */
            AtlasDeviceManager::getInstance().getDevice(identity).registerNow();

            /* Install alerts on client device */
            AtlasDeviceManager::getInstance().getDevice(identity).pushAlerts();

            return ATLAS_COAP_RESP_OK;
        }
    }

    ATLAS_LOGGER_ERROR("Registration end-point called with no REGISTER commands");

    return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
}

void AtlasRegister::start()
{

    ATLAS_LOGGER_DEBUG("Start registration module");

    /* Add REGISTER and KEEPALIVE resource to CoAP server. This is called by the IoT device in the 
       registration phase and for keepalive pings after that.*/
    AtlasCoapServer::getInstance().addResource(registerResource_);
    AtlasCoapServer::getInstance().addResource(keepAliveResource_);
}

void AtlasRegister::stop()
{
    ATLAS_LOGGER_DEBUG("Stop registration module");
    
    AtlasCoapServer::getInstance().delResource(registerResource_);
    AtlasCoapServer::getInstance().delResource(keepAliveResource_);
}

} // namespace atlas
