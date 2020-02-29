#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasDeviceManager.h"
#include "../logger/AtlasLogger.h"
#include "../scheduler/AtlasScheduler.h"

namespace atlas {

AtlasDeviceManager& AtlasDeviceManager::getInstance()
{
    static AtlasDeviceManager instance;

    return instance;
}

AtlasDeviceManager::AtlasDeviceManager(): deviceCloud_(new AtlasDeviceCloud()) {}

AtlasDevice& AtlasDeviceManager::getDevice(const std::string& identity)
{
    if (devices_.find(identity) == devices_.end()) {
        ATLAS_LOGGER_INFO1("New client device created with identity ", identity);
        devices_[identity] = AtlasDevice(identity, deviceCloud_); 
    }

    return devices_[identity];
}

void AtlasDeviceManager::forEachDevice(std::function<void(AtlasDevice&)> cb)
{
    for (auto it = devices_.begin(); it != devices_.end(); ++it)
        cb(it->second); 
}

} // namespace atlas
