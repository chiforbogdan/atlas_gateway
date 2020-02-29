#include <iostream>
#include "AtlasDeviceCloud.h"
#include "../logger/AtlasLogger.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../identity/AtlasIdentity.h"
#include "../device/AtlasDeviceManager.h"

namespace atlas {

namespace {

const int ATLAS_SYNC_TIMER_INTERVAL_MS = 10000;

const std::string ATLAS_CMD_TYPE_JSON_KEY = "commandType";
const std::string ATLAS_CMD_PAYLOAD_JSON_KEY = "commandPayload";
const std::string ATLAS_CMD_IDENTITY_JSON_KEY = "identity";

const std::string ATLAS_CMD_CLIENT_INFO_UPDATE = "ATLAS_CMD_CLIENT_INFO_UPDATE";

} // anonymous namespace

AtlasDeviceCloud::AtlasDeviceCloud() : syncAlarm_(ATLAS_SYNC_TIMER_INTERVAL_MS, false, boost::bind(&AtlasDeviceCloud::syncAlarmCallback, this)),
                                       syncScheduled_(false) {}

void AtlasDeviceCloud::updateDevice(const std::string &identity, const std::string &jsonInfo)
{
    ATLAS_LOGGER_INFO("Update to cloud back-end information for device with identity " + identity);

    std::string cmd = "{\n";

    /* Add header */
    cmd += "\"" + ATLAS_CMD_TYPE_JSON_KEY + "\": \"" + ATLAS_CMD_CLIENT_INFO_UPDATE + "\",";
    cmd += "\n\"" + ATLAS_CMD_PAYLOAD_JSON_KEY + "\": \n";
    cmd += "{\n";

    /* Add identity */
    cmd += "\"" + ATLAS_CMD_IDENTITY_JSON_KEY + "\": \"" + identity + "\",";
    
    /* Add update information */
    cmd +=  "\n" + jsonInfo;
    
    cmd += "\n}\n}";
    
    /* Send to cloud last update info of a registered node*/
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk(), cmd);
    
    /* If message is not delivered, then schedule a full cloud sync */
    if (!delivered) {
        ATLAS_LOGGER_ERROR("Cloud message for identity " + identity + " cannot be delivered. Scheduling full sync...");
        
        if (!syncScheduled_) {
            ATLAS_LOGGER_INFO("Schedule full cloud sync");
            syncAlarm_.start();
            syncScheduled_ = true;
        }
    }
}

void AtlasDeviceCloud::syncAlarmCallback()
{
    ATLAS_LOGGER_INFO("Alarm triggered for cloud sync");

    syncScheduled_ = false;
    
    AtlasDeviceManager::getInstance().forEachDevice([this] (AtlasDevice& device)
                                                       {
                                                           ATLAS_LOGGER_INFO("Executing full cloud sync for device with identity " + device.getIdentity());
                                                           this->updateDevice(device.getIdentity(), device.toJSON());
                                                       }
                                                   );

}

} // namespace atlas
