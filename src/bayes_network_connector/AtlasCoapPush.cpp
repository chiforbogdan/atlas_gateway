#include <iostream>
#include <boost/bind.hpp>
#include "AtlasCoapPush.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

const std::string ATLAS_FEATURE_SEND_URI = "client/feature";

AtlasCoapPush::AtlasCoapPush(const std::string deviceIdentity)
{
    deviceIdentity_ = deviceIdentity;
}

void AtlasCoapPush::respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
{
    ATLAS_LOGGER_INFO1("Feature reputation CoAP response for client with identity ", deviceIdentity_);

    /* If request is successful, there is nothing to do */
    if (respStatus == ATLAS_COAP_RESP_OK) {
        ATLAS_LOGGER_INFO1("Feature reputation COAP received by device with identity ", deviceIdentity_);
        return;
    }

    /* If client processed this request and returned an error, then skip it */
    if (respStatus != ATLAS_COAP_RESP_TIMEOUT) {
        ATLAS_LOGGER_INFO("Feature reputation URI error on the client side. Abording request...");
        return;
    }

    /* Try to push again the feature reputation to device */
    push();
}

void AtlasCoapPush::push()
{
    AtlasDevice &device = AtlasDeviceManager::getInstance().getDevice(deviceIdentity_);
    std::string url = device.getUrl() + "/" + ATLAS_FEATURE_SEND_URI;

    ATLAS_LOGGER_DEBUG("Creating command for feature reputation");

    if (!device.isRegistered()) {
        ATLAS_LOGGER_INFO1("Cannot push feature reputation for OFFLINE device with identity ", deviceIdentity_);
        return;
    }

    /* Set DTLS information for this client device */
    AtlasCoapClient::getInstance().setDtlsInfo(deviceIdentity_, device.getPsk());

    pushCommand(url);
}

void AtlasCoapPush::pushCommand(const std::string &url)
{
    AtlasCommandBatch cmdBatch;
    uint16_t featureReputation = 20;
    featureReputation = htons(featureReputation);
    std::pair<const uint8_t*, size_t> cmdBuf;

    ATLAS_LOGGER_DEBUG("Sending feature reputation to device...");

    /* Add feature command */
    AtlasCommand cmdPush(ATLAS_CMD_FEATURE_REQUEST, sizeof(uint16_t), (uint8_t *) &featureReputation);

    cmdBatch.addCommand(cmdPush);
    cmdBuf = cmdBatch.getSerializedAddedCommands();

    /* Send CoAP request */
    AtlasCoapClient::getInstance().sendRequest(url, ATLAS_COAP_METHOD_PUT, cmdBuf.first, cmdBuf.second,
                                               ATLAS_COAP_TIMEOUT_MS, boost::bind(&AtlasCoapPush::respCallback, this, _1, _2, _3));

    /* FIXME what if object is destroyed while response is hanging and callback reaches the dead object */
}

} // namespace atlas

