#include <boost/bind.hpp>
#include "AtlasCloudCmdParser.h"
#include "AtlasCommandsCloud.h"
#include "../logger/AtlasLogger.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../identity/AtlasIdentity.h"


namespace atlas {

namespace {

const int ATLAS_CONNECTED_CLOUD_INTERVAL_MS = 2000;

} // anonymous namespace

AtlasCloudCmdParser::AtlasCloudCmdParser() : connected_(false),
                                         cAlarm_(ATLAS_CONNECTED_CLOUD_INTERVAL_MS, false, boost::bind(&AtlasCloudCmdParser::connectAlarmCb, this)){}

AtlasCloudCmdParser& AtlasCloudCmdParser::getInstance()
{
    static AtlasCloudCmdParser instance;
    return instance;
}

void AtlasCloudCmdParser::connectAlarmCb()
{
    ATLAS_LOGGER_INFO("Cloud connect-subscribing alarm triggered");

    if (!connected_)
        connected_ = AtlasMqttClient::getInstance().subscribeTopic(AtlasIdentity::getInstance().getPsk()+"-control", 1);
    else
        cAlarm_.cancel();
}

void AtlasCloudCmdParser::CommandGetAllDevicesCallback()
{
    std::string cmd = "{\n";
    
    ATLAS_LOGGER_INFO("Send GET_ALL_DEVICES command to cloud back-end");

    /* Add header */
    cmd += "\"" + ATLAS_CMD_TYPE_JSON_KEY + "\": \"" + ATLAS_CMD_GET_ALL_DEVICES + "\"";
    cmd += "\n}";

    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk(), cmd);
    if (delivered) {
        ATLAS_LOGGER_INFO("GET_ALL_DEVICES command was sent to cloud back-end. Sync all devices...");
        AtlasDeviceManager::getInstance().getDeviceCloud()->allDevicesUpdate();
    }
}

void AtlasCloudCmdParser::start()
{
    ATLAS_LOGGER_DEBUG("Start cloud command parser");

    /*commands registeration */
    addCmdTypeCallback(ATLAS_CMD_GET_ALL_DEVICES, boost::bind(&AtlasCloudCmdParser::CommandGetAllDevicesCallback, this));

    /*start alarm for async subscription*/
    cAlarm_.start();
}

void AtlasCloudCmdParser::parseCmd(std::string const &cmd)
{
    Json::Reader reader;
    Json::Value obj;

    reader.parse(cmd, obj);
    callbacks_[obj["cmd"].asString()]();
}

void AtlasCloudCmdParser::addCmdTypeCallback(std::string const &cmdType, std::function<void()> callback)
{
    callbacks_[cmdType] = callback;
}

void AtlasCloudCmdParser::delCmdTypeCallback(std::string const &cmdType)
{
    callbacks_.erase(cmdType);
}

} // namespace atlas
