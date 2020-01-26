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
    AtlasAlert *alert = nullptr;

    ATLAS_LOGGER_INFO1("Install default telemetry alerts for client with identity ", identity_);

    /* Install default alerts */
    alert = AtlasAlertFactory::getAlert(TELEMETRY_SYSINFO_PROCS, identity_);
    telemetryAlerts_[TELEMETRY_SYSINFO_PROCS] = std::unique_ptr<AtlasAlert>(alert);
}

void AtlasDevice::pushAlerts()
{
    ATLAS_LOGGER_INFO1("Push all telemetry alerts to client with identity ", identity_);

    std::unordered_map<std::string, std::unique_ptr<AtlasAlert>>::iterator it = telemetryAlerts_.begin();
    while (it != telemetryAlerts_.end()) {
        ATLAS_LOGGER_INFO1("Push to client device telemetry alert of type ", (*it).first);
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
