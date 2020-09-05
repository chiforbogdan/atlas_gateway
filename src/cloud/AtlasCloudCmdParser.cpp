#include <boost/bind.hpp>
#include "AtlasCloudCmdParser.h"
#include "AtlasCommandsCloud.h"
#include "AtlasRegisterCloud.h"
#include "../logger/AtlasLogger.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../identity/AtlasIdentity.h"
#include "../claim_approve/AtlasApprove.h"

namespace atlas {

AtlasCloudCmdParser::AtlasCloudCmdParser() : connected_(false) {}

AtlasCloudCmdParser& AtlasCloudCmdParser::getInstance()
{
    static AtlasCloudCmdParser instance;
    return instance;
}

void AtlasCloudCmdParser::getAllDevicesCmd()
{
    ATLAS_LOGGER_INFO("ATLAS_CMD_GATEWAY_GET_ALL_DEVICES command was sent by cloud back-end. Sync all devices...");
    AtlasDeviceManager::getInstance().getDeviceCloud()->allDevicesUpdate();
}

void AtlasCloudCmdParser::reqRegisterCmd()
{
    ATLAS_LOGGER_INFO("ATLAS_CMD_GATEWAY_REGISTER_REQUEST command was sent by cloud back-end");
    AtlasRegisterCloud::getInstance().sendRegisterCmd();
}

void AtlasCloudCmdParser::deviceApprovedCmd(const Json::Value &cmdPayload)
{
    ATLAS_LOGGER_INFO("ATLAS_CMD_GATEWAY_CLIENT command was sent by cloud back-end");
    
    bool result = AtlasApprove::getInstance().checkCommandPayload(cmdPayload);
    if(!result) {
        ATLAS_LOGGER_ERROR("ATLAS_CMD_GATEWAY_CLIENT command returned an error");
    }

}

void AtlasCloudCmdParser::rcvACKForDONEDeviceCommand(const std::string &cmdPayload)
{
    ATLAS_LOGGER_INFO("ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND command was sent by cloud back-end");
    
    /* the command payload contains only the client identity */
    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(cmdPayload);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists with identity " + cmdPayload);
        return;
    }

    if(!device->GetQExecCommands().empty()) {
        AtlasCommandDevice cmd = device->GetQExecCommands().top();
        device->GetQExecCommands().pop();

        bool result = AtlasSQLite::getInstance().deleteDeviceCommand(cmd.getSequenceNumber());
        if(!result) {
            ATLAS_LOGGER_ERROR("ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND error on delete a device command in database");
        }
    } else {
        ATLAS_LOGGER_ERROR("ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND Q of executed device commands is empty");
    }
}

void AtlasCloudCmdParser::start()
{
    ATLAS_LOGGER_DEBUG("Start cloud command parser");

    AtlasMqttClient::getInstance().addConnectionCb(this);
}

void AtlasCloudCmdParser::parseCmd(const std::string &cmd)
{
    Json::Reader reader;
    Json::Value obj;

    reader.parse(cmd, obj);

    if(obj[ATLAS_CMD_TYPE_JSON_KEY].asString() == ATLAS_CMD_GATEWAY_GET_ALL_DEVICES)
        getAllDevicesCmd();
    else if (obj[ATLAS_CMD_TYPE_JSON_KEY].asString() == ATLAS_CMD_GATEWAY_REGISTER_REQUEST)
        reqRegisterCmd();
    else if (obj[ATLAS_CMD_TYPE_JSON_KEY].asString() == ATLAS_CMD_GATEWAY_CLIENT)
        deviceApprovedCmd(obj[ATLAS_CMD_PAYLOAD_JSON_KEY]);
    else if (obj[ATLAS_CMD_TYPE_JSON_KEY].asString() == ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND)
        rcvACKForDONEDeviceCommand(obj[ATLAS_CMD_PAYLOAD_JSON_KEY].asString());
}

void AtlasCloudCmdParser::onConnect()
{
    ATLAS_LOGGER_INFO("Connect event to cloud MQTT broker");

    if (!connected_)
        connected_ = AtlasMqttClient::getInstance().subscribeTopic(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_GATEWAY_TOPIC);
}

void AtlasCloudCmdParser::onDisconnect()
{
    ATLAS_LOGGER_INFO("Disconnect event from cloud MQTT broker");

    connected_ = false;
}

void AtlasCloudCmdParser::stop()
{
    ATLAS_LOGGER_DEBUG("Stop cloud command parser");

    AtlasMqttClient::getInstance().removeConnectionCb(this);
}

} // namespace atlas
