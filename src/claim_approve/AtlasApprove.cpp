#include "AtlasApprove.h"
#include "AtlasClaim.h"
#include <json/json.h>
#include "../sql/AtlasSQLite.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../device/AtlasDevice.h"
#include "../cloud/AtlasCommandsCloud.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../identity/AtlasIdentity.h"

namespace atlas {

namespace {

const std::string ATLAS_CLAIM_REQUEST_PATH = "/gateway/claim";

/* JSON fields from the incoming approve messages */
const std::string ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY = "clientIdentity";
const std::string ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY = "type";
const std::string ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY = "payload";
const std::string ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY = "identifier";

/* Check Q at every 1 minute */
const int ATLAS_APPROVE_ALARM_PERIOD_MS = 60000; 

} // anonymous namespace

uint32_t AtlasApprove::sequenceNumber_ = 0; 

AtlasApprove::AtlasApprove():pushCommandAlarm_("AtlasApprove", ATLAS_APPROVE_ALARM_PERIOD_MS, false,
                                                boost::bind(&AtlasApprove::alarmCallback, this)){}

AtlasApprove& AtlasApprove::getInstance()
{
    static AtlasApprove instance;

    return instance;
}

void AtlasApprove::start()
{
    ATLAS_LOGGER_DEBUG("Start device approved command module");
    pushCommandAlarm_.start();
}


void AtlasApprove::stop()
{
    ATLAS_LOGGER_DEBUG("Stop device approved command module");
    pushCommandAlarm_.cancel();
}

void AtlasApprove::alarmCallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for approve protocol");

    /* Push top command for each device */
    AtlasDeviceManager::getInstance().forEachDevice([] (AtlasDevice& device)
                                                    { 
                                                        if(!device.GetQCommands().empty()) {
                                                            AtlasCommandDevice cmd = device.GetQCommands().top();
                                                            cmd.pushCommand();
                                                        }
                                                    });
}

void AtlasApprove::checkCommandPayload(const std::string &payload)
{
    
    if(!AtlasClaim::getInstance().isClaimed()) {
        ATLAS_LOGGER_INFO("Received a command for an unclaimed gateway!");
        /* If gateway is unclaimed, the received command does not need to be authorized*/
    }
    else {
        ATLAS_LOGGER_INFO("Received an approved command for a claimed gateway!");
        /* If gateway is claimed, the received command need to be authorized(HMAC)*/
        //TODO - check HMAC
    }

    if(payload == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty payload!");
        return;
    }

    Json::Reader reader;
    Json::Value obj;

    reader.parse(payload, obj);

    if (obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty sequence number field!");
        return;
    }

    if (obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt() <= sequenceNumber_) {
        ATLAS_LOGGER_ERROR("Received a command in which the sequence number is less or equal than current sequence number!");
        /*  Check if the received command is already in db 
            This is a case when the back-end cloud did not recevied the ACK for first sending of this command */

        bool result = AtlasSQLite::getInstance().checkDeviceCommand(obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());

        if(result) {

            result = AtlasSQLite::getInstance().updateDeviceCommand(obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());

            if(result) {
                /* Send directly DONE status if the command is already executed and the cloud did not received the ACK status until now*/
                ResponseCommandDONE(obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());
            }

            ResponseCommandACK(obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());
        }

        return;
    } 
    sequenceNumber_ = obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();
    
    if (obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty device id field!");
        return;	
    }

    if (obj[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an command type field!");
        return;
    } 

    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists with identity " + obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
        return;
    }

    /* Save command into device Q */
    AtlasCommandDevice cmd(obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString(),
                           obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                           obj[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                           obj[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString());

    device->GetQCommands().push(std::move(cmd));

    /* Save command into the database */
    bool result = AtlasSQLite::getInstance().insertDeviceCommand(obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                                                                 obj[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                                                                 obj[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString(),
                                                                 obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if(!result) {
        ATLAS_LOGGER_ERROR("Cannot save device command into the database!");
	    return;
    }

    ResponseCommandACK(sequenceNumber_);
}

void AtlasApprove::ResponseCommandACK(const uint32_t sequenceNumber)
{
    /* ACK response is sent after each command is received */
    std::string cmd = "{\n";
    
    ATLAS_LOGGER_INFO("Send ACK response to cloud back-end");

    /* Add header */
    cmd += "\"" + ATLAS_CMD_TYPE_JSON_KEY + "\": \"" + ATLAS_CMD_GATEWAY_CLIENT_ACK + "\"";
    cmd += "\n\"" + ATLAS_CMD_PAYLOAD_JSON_KEY + "\": \"" + std::to_string(sequenceNumber) + "\"";
    cmd += "\n}";

    /* Send ACK command */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, cmd);
    if (!delivered)
        ATLAS_LOGGER_ERROR("ACK command was not sent to cloud back-end");
}

void AtlasApprove::ResponseCommandDONE(const uint32_t sequenceNumber)
{
    /* DONE response is sent after one command is executed on targeted device */
    std::string cmd = "{\n";
    
    ATLAS_LOGGER_INFO("Send DONE response to cloud back-end");

    /* Add header */
    cmd += "\"" + ATLAS_CMD_TYPE_JSON_KEY + "\": \"" + ATLAS_CMD_GATEWAY_CLIENT_DONE + "\"";
    cmd += "\n\"" + ATLAS_CMD_PAYLOAD_JSON_KEY + "\": \"" + std::to_string(sequenceNumber) + "\"";
    cmd += "\n}";

    /* Send DONE command */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, cmd);
    if (!delivered)
        ATLAS_LOGGER_ERROR("DONE command was not sent to cloud back-end");
}

} // namespace atlas
