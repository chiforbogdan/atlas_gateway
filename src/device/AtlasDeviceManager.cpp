#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasDeviceManager.h"
#include "../logger/AtlasLogger.h"
#include "../scheduler/AtlasScheduler.h"

namespace atlas {

namespace {

const int ATLAS_FIREWALL_STATISTICS_INTERVAL_MS = 60000;

} // anonymous namespace

AtlasDeviceManager& AtlasDeviceManager::getInstance()
{
    static AtlasDeviceManager instance;

    return instance;
}

AtlasDeviceManager::AtlasDeviceManager(): deviceCloud_(new AtlasDeviceCloud()),
                                          fsAlarm_(ATLAS_FIREWALL_STATISTICS_INTERVAL_MS, false, boost::bind(&AtlasDeviceManager::firewallStatisticsAlarmCallback, this)) {}

void AtlasDeviceManager::firewallStatisticsAlarmCallback()
{
    ATLAS_LOGGER_INFO("Firewall-statistics alarm callback");

    AtlasDeviceManager::getInstance().forEachDevice([] (AtlasDevice& device)
                                                        { 
                                                            device.getFirewallRuleStats();
                                                        });
} 

void AtlasDeviceManager::startFirewallStatisticsAlarm()
{
    fsAlarm_.start();
}

void AtlasDeviceManager::stopFirewallStatisticsAlarm()
{
    fsAlarm_.cancel();
}

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
