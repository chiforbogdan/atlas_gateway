
#include <iostream>
#include <boost/bind.hpp>
#include "AtlasCommandDevice.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapClient.h"
#include "AtlasCommandBatch.h"
#include "AtlasCommandType.h"
#include "../device/AtlasDeviceManager.h"
#include "../claim_approve/AtlasApprove.h"
#include "../sql/AtlasSQLite.h"

namespace atlas {

namespace {

const uint16_t ATLAS_APPROVED_COMMAND_COAP_TIMEOUT_MS = 5000;
const std::string ATLAS_CMD_DEVICE_RESTART_CLOUD = "ATLAS_CMD_DEVICE_RESTART";
const std::string ATLAS_CMD_DEVICE_SHUTDOWN_CLOUD = "ATLAS_CMD_DEVICE_SHUTDOWN";
const std::string ATLAS_PUSH_APPROVED_COMMAND = "client/approved/command/push";

/* Number of accepted timeouts before stop sending device command */
const uint8_t ATLAS_PUSH_COMMAND_RETRY_NO = 3; 

} // anonymous namespace


AtlasCommandDevice::AtlasCommandDevice( const std::string &deviceIdentity, const uint32_t sequenceNumber, 
                                        const std::string &commandType, const std::string &commandPayload) : 
                                        coapToken_(nullptr), deviceIdentity_(deviceIdentity), 
                                        sequenceNumber_(sequenceNumber), commandTypeCloud_(commandType), 
                                        commandPayload_(commandPayload), counterTimeouts_(0)
{
    if(commandType == ATLAS_CMD_DEVICE_RESTART_CLOUD) {
        commandTypeDevice_ = ATLAS_CMD_DEVICE_RESTART;
    } else if(commandType == ATLAS_CMD_DEVICE_SHUTDOWN_CLOUD) {
        commandTypeDevice_ = ATLAS_CMD_DEVICE_SHUTDOWN;
    } else {
        commandTypeDevice_ = ATLAS_CMD_DEVICE_UNKNOWN;
    }
}

void AtlasCommandDevice::pushCommand()
{
    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(deviceIdentity_);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + deviceIdentity_);
        return;
    }
    
    std::string url = device->getUrl() + "/" + ATLAS_PUSH_APPROVED_COMMAND;

    ATLAS_LOGGER_DEBUG("Creating command for device");

    if (!device->isRegistered()) {
        ATLAS_LOGGER_INFO1("Cannot push command for OFFLINE device with identity ", deviceIdentity_);
        return;
    }

    if (commandTypeDevice_ == ATLAS_CMD_DEVICE_UNKNOWN) {
        ATLAS_LOGGER_ERROR("Unknown commmand type received from cloud for device with identity " + deviceIdentity_);
        return;
    }

    /* Set DTLS information for this client device */
    AtlasCoapClient::getInstance().setDtlsInfo(deviceIdentity_, device->getPsk());

    AtlasCommandBatch cmdBatch;
    std::pair<const uint8_t*, size_t> cmdBuf;

    ATLAS_LOGGER_DEBUG("Sending command to device...");

    /* Add command */
    AtlasCommand cmd(commandTypeDevice_, 0, nullptr);

    cmdBatch.addCommand(cmd);
    cmdBuf = cmdBatch.getSerializedAddedCommands();

    try
    {
        /* Send CoAP request */
        coapToken_ = AtlasCoapClient::getInstance().sendRequest(url, ATLAS_COAP_METHOD_PUT, cmdBuf.first, cmdBuf.second,
                                                ATLAS_ALERT_COAP_TIMEOUT_MS, boost::bind(&AtlasCommandDevice::respCallback, this, _1, _2, _3));
    }
    catch(const char *e)
    {
        ATLAS_LOGGER_ERROR(e);
    }
}

void AtlasCommandDevice::respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
{
    ATLAS_LOGGER_INFO1("Command CoAP response for client with identity ", deviceIdentity_);

    /* If request is successful, the command need to be marked */
    if (respStatus == ATLAS_COAP_RESP_OK) {
        ATLAS_LOGGER_INFO1("Command executed on device with identity ", deviceIdentity_);
        /* Change status in local db*/

        /* Reset number of timeouts */
        if(counterTimeouts_ != 0) {
            counterTimeouts_ = 0;
        }
        
        if(!AtlasDeviceManager::getInstance().getDevice(deviceIdentity_)->GetQRecvCommands().empty()) {
            /* Transfer the device command from recv to exec Q */
            AtlasCommandDevice cmd = AtlasDeviceManager::getInstance().getDevice(deviceIdentity_)->GetQRecvCommands().top();
            AtlasDeviceManager::getInstance().getDevice(deviceIdentity_)->GetQRecvCommands().pop();
            AtlasDeviceManager::getInstance().getDevice(deviceIdentity_)->GetQExecCommands().push(std::move(cmd));
        }

        bool result = AtlasSQLite::getInstance().markExecutedDeviceCommand(sequenceNumber_);
        if(!result) {
            ATLAS_LOGGER_ERROR("Cannot update device command as executed into the database!");
            return;
        }

        /* Set only if there is no scheduled DONE message */
        if(!AtlasApprove::getInstance().getMsgDONEScheduled()) {
            AtlasApprove::getInstance().setSequenceNumberDONE(sequenceNumber_);

            result = AtlasApprove::getInstance().responseCommandDONE();
            if(!result) {
                ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
                return;
            }    
        }

        return;
    }

    /* If client processed this request and returned an error */
    if (respStatus != ATLAS_COAP_RESP_TIMEOUT) {
        ATLAS_LOGGER_INFO("Command URI error on the client side. Abording request...");

        /* Reset number of timeouts */
        if(counterTimeouts_ != 0) {
            counterTimeouts_ = 0;
        }
        return;
    }

    if(counterTimeouts_ == ATLAS_PUSH_COMMAND_RETRY_NO) {
        ATLAS_LOGGER_ERROR("Client is in an error state. Abort resending command");
        
        /* Reset number of timeouts */
        if(counterTimeouts_ != 0) {
            counterTimeouts_ = 0;
        }

        return;
    }

    counterTimeouts_++;

    /* Try to push again the command to device*/
    pushCommand();
}



AtlasCommandDevice::~AtlasCommandDevice()
{
    /* Destroy reference callbacks to this instance*/
    AtlasCoapClient::getInstance().cancelRequest(coapToken_);
}

} // namespace atlas
