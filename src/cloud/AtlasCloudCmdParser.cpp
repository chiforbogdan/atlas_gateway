#include <boost/bind.hpp>
#include "AtlasCloudCmdParser.h"
#include "AtlasCommandsCloud.h"
#include "AtlasRegisterCloud.h"
#include "../logger/AtlasLogger.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../identity/AtlasIdentity.h"

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


void AtlasCloudCmdParser::start()
{
    ATLAS_LOGGER_DEBUG("Start cloud command parser");

    AtlasMqttClient::getInstance().addConnectionCb(this);
}

void AtlasCloudCmdParser::parseCmd(std::string const &cmd)
{
    Json::Reader reader;
    Json::Value obj;

    reader.parse(cmd, obj);
    if(obj[ATLAS_CMD_TYPE_JSON_KEY].asString() == ATLAS_CMD_GATEWAY_GET_ALL_DEVICES)
        getAllDevicesCmd();
    else if (obj[ATLAS_CMD_TYPE_JSON_KEY].asString() == ATLAS_CMD_GATEWAY_REGISTER_REQUEST)
        reqRegisterCmd();
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
