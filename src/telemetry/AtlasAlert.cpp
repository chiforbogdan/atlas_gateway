#include <iostream>
#include <boost/bind.hpp>
#include "AtlasAlert.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

const uint16_t ATLAS_ALERT_COAP_TIMEOUT_MS = 5000;

AtlasAlert::AtlasAlert(const std::string &deviceIdentity, const std::string &path,
                       uint16_t extPushRate, uint16_t intScanRate, const std::string &threshold)
{
    deviceIdentity_ = deviceIdentity;
    path_ = path;
    extPushRate_ = extPushRate;
    intScanRate_ = intScanRate;
    threshold_ = threshold;
}

void AtlasAlert::respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
{
    ATLAS_LOGGER_INFO1("Telemetry alert CoAP response for client with identity ", deviceIdentity_);

    /* If request is successful, there is nothing to do */
    if (respStatus == ATLAS_COAP_RESP_OK) {
        ATLAS_LOGGER_INFO1("Telemetry alert installed for device with identity ", deviceIdentity_);
        return;
    }

    /* If gateway processed this request and returned an error, then skip it */
    if (respStatus != ATLAS_COAP_RESP_TIMEOUT) {
        ATLAS_LOGGER_INFO("Telemetry alert URI error on the client side. Abording request...");
        return;
    }

    /* Try to push again the telemetry alert to device */
    push();
}

void AtlasAlert::push()
{
    AtlasCommandBatch cmdBatch;
    AtlasDevice &device = AtlasDeviceManager::getInstance().getDevice(deviceIdentity_);
    std::string url = device.getUrl() + "/" + path_;
    uint16_t extPush = htons(extPushRate_);
    uint16_t intScan = htons(intScanRate_);
    std::pair<const uint8_t*, size_t> cmdBuf;

    ATLAS_LOGGER_DEBUG("Creating command for telemetry alert push");

    if (!device.isRegistered()) {
        ATLAS_LOGGER_INFO1("Cannot push telemetry alert for OFFLINE device with identity ", deviceIdentity_);
        return;
    }

    /* Add telemetry alert commands */
    AtlasCommand cmdExtPush(ATLAS_CMD_TELEMETRY_ALERT_EXT_PUSH_RATE, sizeof(uint16_t), (uint8_t *) &extPush);
    AtlasCommand cmdIntScan(ATLAS_CMD_TELEMETRY_ALERT_INT_SCAN_RATE, sizeof(uint16_t), (uint8_t *) &intScan);
    AtlasCommand cmdThreshold(ATLAS_CMD_TELEMETRY_ALERT_THRESHOLD, threshold_.length(), (uint8_t *) threshold_.c_str());

    cmdBatch.addCommand(cmdExtPush);
    cmdBatch.addCommand(cmdIntScan);
    cmdBatch.addCommand(cmdThreshold);
    cmdBuf =  cmdBatch.getSerializedAddedCommands();

    /* Set DTLS information for this client device */
    AtlasCoapClient::getInstance().setDtlsInfo(deviceIdentity_, device.getPsk());     

    /* Send CoAP request */
    AtlasCoapClient::getInstance().sendRequest(url, ATLAS_COAP_METHOD_PUT, cmdBuf.first, cmdBuf.second,
                                               ATLAS_ALERT_COAP_TIMEOUT_MS, boost::bind(&AtlasAlert::respCallback, this, _1, _2, _3));

    /* FIXME what if object is destroyed while response is hanging and callback reaches the dead object */
}

} // namespace atlas

