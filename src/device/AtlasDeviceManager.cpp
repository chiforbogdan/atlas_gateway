#include "AtlasDeviceManager.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

AtlasDeviceManager& AtlasDeviceManager::getInstance()
{
    static AtlasDeviceManager instance;

    return instance;
}

AtlasDevice& AtlasDeviceManager::getDevice(const std::string& identity)
{
    AtlasDevice& device = devices_[identity];

    if (device.getIdentity() == "") {
        device.setIdentity(identity);
        ATLAS_LOGGER_INFO1("New client device created with identity ", identity);
    }

    return device;
}

} // namespace atlas
