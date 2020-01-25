#include <string>
#include <vector>
#include <boost/bind.hpp>
#include "AtlasTelemetry.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapServer.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

const std::string ATLAS_TELEMETRY_HOSTNAME_URI = "gateway/telemetry/hostname";
const std::string ATLAS_TELEMETRY_KERN_INFO_URI = "gateway/telemetry/kernel_info";

AtlasTelemetry::AtlasTelemetry() : hostnameResource_(ATLAS_TELEMETRY_HOSTNAME_URI,
                                                     ATLAS_COAP_METHOD_PUT,
                                                     boost::bind(&AtlasTelemetry::featureCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   kernInfoResource_(ATLAS_TELEMETRY_KERN_INFO_URI,
                                                     ATLAS_COAP_METHOD_PUT,
                                                     boost::bind(&AtlasTelemetry::featureCallback, this, _1, _2, _3, _4, _5, _6, _7, _8))
{
    ATLAS_LOGGER_DEBUG("Start telemetry module");
 
    /* Add CoAP resource for each telemetry feature */
    AtlasCoapServer::getInstance().addResource(hostnameResource_);
    AtlasCoapServer::getInstance().addResource(kernInfoResource_);
}

std::pair<std::string,std::string> AtlasTelemetry::getFeature(const std::string &path, AtlasCommand &cmd)
{
    std::pair<std::string, std::string> ret;
    ret.first = "";
    ret.second = "";

    if (!cmd.getLen()) {
        ATLAS_LOGGER_ERROR("Telemetry end-point called with invalid feature length");
        return ret;
    }
 
    if (std::strstr(path.c_str(), ATLAS_TELEMETRY_HOSTNAME_URI.c_str()) && cmd.getType() == ATLAS_CMD_TELEMETRY_HOSTNAME)
        ret.first = TELEMETRY_HOSTNAME;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_KERN_INFO_URI.c_str()) && cmd.getType() == ATLAS_CMD_TELEMETRY_KERN_INFO)
        ret.first = TELEMETRY_KERN_INFO;
        
    ret.second.assign((char *)cmd.getVal(), cmd.getLen());

    return ret;
}

AtlasCoapResponse AtlasTelemetry::featureCallback(const std::string &path, const std::string &pskIdentity,
                                                  const std::string& psk, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string identity = "";
    std::pair<std::string, std::string> feature;
    
    ATLAS_LOGGER_DEBUG("Telemetry hostname callback executed...");

    ATLAS_LOGGER_INFO1("Process TELEMETRY command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice& device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if (!device.isRegistered()) {
        ATLAS_LOGGER_ERROR("Received TELEMETRY command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Telemetry end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Telemetry end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Telemetry hostname end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } else
            feature = getFeature(path, cmdEntry);
    }

    if (identity == "") {
        ATLAS_LOGGER_ERROR("Telemetry failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    if (feature.first == "" || feature.second == "") {
        ATLAS_LOGGER_ERROR("Telemetry failed because of invalid feature");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Set device telemetry feature */
    device.getTelemetryInfo().setFeature(feature.first, feature.second);

    ATLAS_LOGGER_INFO1("Device telemetry state changed to ", device.getTelemetryInfo().toString());
    
    return ATLAS_COAP_RESP_OK;
}

} // namespace atlas
