#include "AtlasApprove.h"
#include "AtlasClaim.h"
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

/* Check Q at every 5 minute */
const int ATLAS_PUSH_COMMAND_ALARM_PERIOD_MS = 300000;

/* Resend status at each 10 sec */
const int ATLAS_STATUS_COMMAND_ALARM_PERIOD_MS = 10000; 

/* Retry sending status for 3 times */
const uint8_t ATLAS_STATUS_COMMAND_RETRY_NO = 3; 

} // anonymous namespace

AtlasApprove::AtlasApprove(): pushCommandAlarm_("AtlasApprove", ATLAS_PUSH_COMMAND_ALARM_PERIOD_MS, false,
                                                boost::bind(&AtlasApprove::alarmCallback, this)),
                              statusACKAlarm_("AtlasApprove", ATLAS_STATUS_COMMAND_ALARM_PERIOD_MS, false,
                                                boost::bind(&AtlasApprove::statusACKCallback, this)),
                              statusDONEAlarm_("AtlasApprove", ATLAS_STATUS_COMMAND_ALARM_PERIOD_MS, false,
                                                boost::bind(&AtlasApprove::statusDONECallback, this)),
                              sequenceNumber_(0), sequenceNumberDONE_(0),
                              msgACKScheduled_(false), msgDONEScheduled_(false),
                              counterACK_(0), counterDONE_(0) {}

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
    ATLAS_LOGGER_DEBUG("Execute alarm for pushing device commands to clients");

    /* Push top command for each device */
    AtlasDeviceManager::getInstance().forEachDevice([] (AtlasDevice& device)
                                                    {
                                                        /* Event for sending device commands to client*/ 
                                                        if(!device.GetQRecvCommands().empty()) {
                                                            AtlasCommandDevice cmd = device.GetQRecvCommands().top();
                                                            cmd.pushCommand();
                                                        }

                                                        /* Event for sending DONE notification for executed device commands to cloud (got notified by client in a scheduled DONE window) */
                                                        if(!AtlasApprove::getInstance().getMsgDONEScheduled() && !device.GetQExecCommands().empty()) {
                                                            AtlasCommandDevice cmd = device.GetQExecCommands().top();
                                                            AtlasApprove::getInstance().setSequenceNumberDONE(cmd.getSequenceNumber());
                                                            bool result = AtlasApprove::getInstance().responseCommandDONE();
                                                            if(!result) {
                                                                ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
                                                                return;
                                                            }
                                                        }
                                                    });
                                
}

void AtlasApprove::statusACKCallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for sending ACK status device commands to cloud");

    bool result = responseCommandACK();
    if(!result) {
        ATLAS_LOGGER_INFO("Retry ACK message " + std::to_string(ATLAS_STATUS_COMMAND_RETRY_NO - counterACK_) + " times");
    } else {
        ATLAS_LOGGER_INFO("Stop scheduling ACK message");
        statusACKAlarm_.cancel();
        msgACKScheduled_ = false;
        counterACK_ = 0;
    }

    return;
}

void AtlasApprove::statusDONECallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for sending DONE status device commands to cloud");

    bool result = responseCommandDONE();
    if(!result) {
        ATLAS_LOGGER_INFO("Retry DONE message " + std::to_string(ATLAS_STATUS_COMMAND_RETRY_NO - counterDONE_) + " times");
    } else {
        ATLAS_LOGGER_INFO("Stop scheduling DONE message");
        statusDONEAlarm_.cancel();
        msgDONEScheduled_ = false;
        counterDONE_ = 0;

        result = AtlasSQLite::getInstance().markDoneDeviceCommand(sequenceNumberDONE_);
        if(!result) {
            ATLAS_LOGGER_ERROR("Cannot update device command as DONE into the database!");
            return;
        }
    }
    
    return;
}

bool AtlasApprove::checkCommandPayload(const Json::Value &payload)
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

    if (payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty sequence number field!");
        return false;
    }

    if (payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt() <= sequenceNumber_) {
        ATLAS_LOGGER_ERROR("Received a command in which the sequence number is less or equal than current sequence number!");
        /*  Check if the received command is already in db 
            This is a case when the back-end cloud did not recevied the ACK for first sending of this command */

        bool result = AtlasSQLite::getInstance().checkDeviceCommandBySeqNo(payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());
        if(result) {
            sequenceNumber_ = payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();
            result = AtlasSQLite::getInstance().checkDeviceCommandForExecution(payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());

            if(result) {
                sequenceNumberDONE_ = payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();

                /* Send directly DONE status if the command is already executed and the cloud did not received the ACK status until now*/
                result = responseCommandDONE();

                if(!result) {
                    ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
                    return false;
                }

                return true;
            }

            result = responseCommandACK();
            if(!result) {
                ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
                return false;
            }

            return true;
        }

        return false;
    } 

    sequenceNumber_ = payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();
    
    if (payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty device id field!");
        return false;	
    }

    if (payload[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an command type field!");
        return false;
    } 

    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists with identity " + payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
        return false;
    }

    /* Save command into device Q */
    AtlasCommandDevice cmd(payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString(),
                           payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                           payload[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                           payload[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString());

    device->GetQRecvCommands().push(std::move(cmd));

    /* Save command into the database */
    bool result = AtlasSQLite::getInstance().insertDeviceCommand(payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                                                                 payload[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                                                                 payload[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString(),
                                                                 payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if(!result) {
        ATLAS_LOGGER_ERROR("Cannot save device command into the database!");
	    return false;
    }

    sequenceNumber_ = payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();
    result = responseCommandACK();
    if(!result) {
        ATLAS_LOGGER_ERROR("ACK message was not sent to cloud back-end");
        return false;
    }
    
    return true;
}

bool AtlasApprove::responseCommandACK()
{
    Json::FastWriter fastWriter;
    Json::Value cmd;
    
    /* Set command type */
    cmd[ATLAS_CMD_TYPE_JSON_KEY] = ATLAS_CMD_GATEWAY_CLIENT_ACK;
    /* Set command payload (sequence number) */
    cmd[ATLAS_CMD_PAYLOAD_JSON_KEY][ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY] = sequenceNumber_;

    std::string ackCmd = fastWriter.write(cmd);
    
    ATLAS_LOGGER_INFO("Send ACK response to cloud back-end for command with sequence number: " + std::to_string(sequenceNumber_));

    /* Send ACK message */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, ackCmd);
    
    /* If message is not delivered, then schedule an ACK message */
    if (!delivered) {
        ATLAS_LOGGER_ERROR("ACK message was not sent to cloud back-end");

        if (!msgACKScheduled_) {
            ATLAS_LOGGER_INFO("Schedule ACK message");
            statusACKAlarm_.start();
            msgACKScheduled_ = true;
        }
        
        if(counterACK_ == ATLAS_STATUS_COMMAND_RETRY_NO) {
            ATLAS_LOGGER_INFO("Stop scheduling ACK message");
            statusACKAlarm_.cancel();
            msgACKScheduled_ = false;
            counterACK_ = 0;
        }

        counterACK_++;

    } else {
        ATLAS_LOGGER_INFO("ACK message was sent to cloud back-end");
    }

    return delivered;
}

bool AtlasApprove::responseCommandDONE()
{
    /* DONE response is sent after one command is executed on targeted device */
    std::string cmd = "{\n";
    
    ATLAS_LOGGER_INFO("Send DONE response to cloud back-end");

    /* Add header */
    cmd += "\"" + ATLAS_CMD_TYPE_JSON_KEY + "\": \"" + ATLAS_CMD_GATEWAY_CLIENT_DONE + "\"";
    cmd += "\n\"" + ATLAS_CMD_PAYLOAD_JSON_KEY + "\": \"" + std::to_string(sequenceNumberDONE_) + "\"";
    cmd += "\n}";

    /* Send DONE message */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, cmd);
    if (!delivered) {
        ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
        if (!msgDONEScheduled_) {
            ATLAS_LOGGER_INFO("Schedule DONE message");
            statusDONEAlarm_.start();
            msgDONEScheduled_ = true;
        }

        if(counterDONE_ == ATLAS_STATUS_COMMAND_RETRY_NO) {
            ATLAS_LOGGER_INFO("Stop scheduling DONE message");
            statusDONEAlarm_.cancel();
            msgDONEScheduled_ = false;
            counterDONE_ = 0;
        }

        counterDONE_++;

    } else {
        ATLAS_LOGGER_INFO("DONE message was sent to cloud back-end");

        bool result = AtlasSQLite::getInstance().markDoneDeviceCommand(sequenceNumberDONE_);
        if(!result) {
            ATLAS_LOGGER_ERROR("Cannot update device command as DONE into the database!");
            return false;
        }
    }

    return delivered;
        
}

} // namespace atlas
