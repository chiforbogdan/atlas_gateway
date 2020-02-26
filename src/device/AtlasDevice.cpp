#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasDevice.h"
#include "../logger/AtlasLogger.h"
#include "../telemetry/AtlasTelemetryInfo.h"
#include "../telemetry/AtlasAlertFactory.h"

namespace atlas {

/* Keep-alive counter initial value */
const int ATLAS_KEEP_ALIVE_COUNTER = 6;

AtlasDevice::AtlasDevice(const std::string &identity) : identity_(identity), registered_(false)
{
    installDefaultAlerts();
}

AtlasDevice::AtlasDevice() : identity_(""), registered_(false) {}

void AtlasDevice::installDefaultAlerts()
{
    AtlasPushAlert *pushAlert;
    AtlasThresholdAlert *thresholdAlert;

    ATLAS_LOGGER_INFO1("Install default telemetry alerts for client with identity ", identity_);
    
    /* Install default push alerts */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_PROCS, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_PROCS] = std::unique_ptr<AtlasAlert>(pushAlert);
    
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_UPTIME, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_UPTIME] = std::unique_ptr<AtlasAlert>(pushAlert);

    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE, identity_);
    pushAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE] = std::unique_ptr<AtlasAlert>(pushAlert);

    /* Install default threshold alerts */
    thresholdAlert = AtlasAlertFactory::getThresholdAlert(TELEMETRY_SYSINFO_PROCS, identity_);
    thresholdAlerts_[TELEMETRY_SYSINFO_PROCS] = std::unique_ptr<AtlasAlert>(thresholdAlert);

    thresholdAlert = AtlasAlertFactory::getThresholdAlert(TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE, identity_);
    thresholdAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE] = std::unique_ptr<AtlasAlert>(thresholdAlert);
}

void AtlasDevice::pushAlerts()
{
    ATLAS_LOGGER_INFO1("Push all telemetry alerts to client with identity ", identity_);

    std::unordered_map<std::string, std::unique_ptr<AtlasAlert>>::iterator it = pushAlerts_.begin();
    while (it != pushAlerts_.end()) {
        ATLAS_LOGGER_INFO1("Push to client device telemetry push alert of type ", (*it).first);
        (*it).second->push();
        ++it;
    }
    
    it = thresholdAlerts_.begin();
    while (it != thresholdAlerts_.end()) {
        ATLAS_LOGGER_INFO1("Push to client device telemetry threshold alert of type ", (*it).first);
        (*it).second->push();
        ++it;
    }
}

void AtlasDevice::registerNow()
{
    boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
    regTime_ = boost::posix_time::to_simple_string(timeLocal);

    registered_ = true;
    kaCtr_ = ATLAS_KEEP_ALIVE_COUNTER;

    ATLAS_LOGGER_INFO1("Client device registered at ", regTime_);
}

void AtlasDevice::keepAliveNow()
{
    boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
    keepAliveTime_ = boost::posix_time::to_simple_string(timeLocal);

    kaCtr_ = ATLAS_KEEP_ALIVE_COUNTER;

    ATLAS_LOGGER_INFO1("Client device sent a keep-alive at ", keepAliveTime_);
}

void AtlasDevice::keepAliveExpired()
{
    if (!kaCtr_)
        return;

    kaCtr_--;

    if (!kaCtr_) {
        ATLAS_LOGGER_INFO1("Keep-alive counter expired for client with identity ", identity_);
        registered_ = false;
    }
}

} // namespace atlas
