#include <iostream>
#include <vector>
#include <boost/bind.hpp>
#include "AtlasTelemetry.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapServer.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

const std::string ATLAS_TELEMETRY_HOSTNAME_URI = "gateway/telemetry/hostname";

AtlasTelemetry::AtlasTelemetry() : hostnameResource_(ATLAS_TELEMETRY_HOSTNAME_URI,
                                                     ATLAS_COAP_METHOD_PUT,
                                                     boost::bind(&AtlasTelemetry::hostnameCallback, this, _1, _2, _3, _4, _5, _6, _7, _8))
{
    ATLAS_LOGGER_DEBUG("Start telemetry module");
 
    /* Add CoAP resource for each telemetry feature */
    AtlasCoapServer::getInstance().addResource(hostnameResource_);
}

AtlasCoapResponse AtlasTelemetry::hostnameCallback(const std::string &path, const std::string &pskIdentity,
                                                   const std::string& psk, AtlasCoapMethod method,
                                                   const uint8_t* reqPayload, size_t reqPayloadLen,
                                                   uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string identity = "";
    std::string hostname = "";

    ATLAS_LOGGER_DEBUG("Telemetry hostname callback executed...");

    ATLAS_LOGGER_INFO1("Process TELEMETRY HOSTNAME command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice& device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if (!device.isRegistered()) {
        ATLAS_LOGGER_ERROR("Received TELEMETRY HOSTNAME command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Telemetry hostname end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_TELEMETRY_HOSTNAME) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Telemetry hostname end-point called with invalid HOSTNAME token length");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            hostname.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
        } else if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Telemetry hostname end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Telemetry hostname end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        }
    }

    if (identity == "") {
        ATLAS_LOGGER_ERROR("Telemetry hostname failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    if (hostname == "") {
        ATLAS_LOGGER_ERROR("Telemetry hostname failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Set device hostname */
    device.getTelemetryInfo().setFeature(TELEMETRY_HOSTNAME, hostname);

    ATLAS_LOGGER_INFO1("Device telemetry state changed to ", device.getTelemetryInfo().toString());
    
    return ATLAS_COAP_RESP_OK;
}


} // namespace atlas
