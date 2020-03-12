#include <boost/bind.hpp>
#include "AtlasCloudCmdParser.h"
#include "AtlasCommandsCloud.h"
#include "../logger/AtlasLogger.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../identity/AtlasIdentity.h"


namespace atlas {

namespace {

const std::string ATLAS_SUFFIX_SUBSCRIBE_TOPIC = "-to-cloud";

} // anonymous namespace

AtlasCloudCmdParser::AtlasCloudCmdParser() : connected_(false){}

AtlasCloudCmdParser& AtlasCloudCmdParser::getInstance()
{
    static AtlasCloudCmdParser instance;
    return instance;
}

void AtlasCloudCmdParser::CommandGetAllDevicesCallback()
{
    ATLAS_LOGGER_INFO("GET_ALL_DEVICES command was sent to cloud back-end. Sync all devices...");
    AtlasDeviceManager::getInstance().getDeviceCloud()->allDevicesUpdate();
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
    if(obj["cmdType"].asString() == ATLAS_CMD_GET_ALL_DEVICES) {
        CommandGetAllDevicesCallback();
    }
}

void AtlasCloudCmdParser::onConnect()
{
    ATLAS_LOGGER_INFO("Connect event to cloud MQTT broker");

    if (!connected_)
        connected_ = AtlasMqttClient::getInstance().subscribeTopic(AtlasIdentity::getInstance().getPsk() + ATLAS_SUFFIX_SUBSCRIBE_TOPIC, 1);
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
