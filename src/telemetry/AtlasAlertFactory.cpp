#include "AtlasAlertFactory.h"
#include "AtlasTelemetryInfo.h"

namespace atlas {

namespace {
/* Sysinfo procs default push alert values */
const uint16_t ATLAS_PUSH_ALERT_PROCS_RATE_SEC_DEFAULT = 60;
const std::string ATLAS_PUSH_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/push";
/* Sysinfo procs default threshold alert value */
const uint16_t ATLAS_THRESHOLD_ALERT_PROCS_RATE_SEC_DEFAULT = 2;
const std::string ATLAS_THRESHOLD_ALERT_PROCS_THRESHOLD_DEFAULT = "1000";
const std::string ATLAS_THRESHOLD_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/threshold";

/* Sysinfo uptime default push alert value */
const uint16_t ATLAS_PUSH_ALERT_UPTIME_RATE_SEC_DEFAULT = 120;
const std::string ATLAS_PUSH_ALERT_UPTIME_PATH = "client/telemetry/sysinfo/uptime/alerts/push";

} // namespace anonymous

AtlasPushAlert* AtlasAlertFactory::getPushAlert(const std::string& type, const std::string &deviceIdentity)
{
    if (type == TELEMETRY_SYSINFO_PROCS)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_PROCS_PATH,
                                  ATLAS_PUSH_ALERT_PROCS_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_UPTIME)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_UPTIME_PATH,
                                  ATLAS_PUSH_ALERT_UPTIME_RATE_SEC_DEFAULT);	
    
    return nullptr;
}

AtlasThresholdAlert* AtlasAlertFactory::getThresholdAlert(const std::string& type, const std::string &deviceIdentity)
{
    if (type == TELEMETRY_SYSINFO_PROCS)
        return new AtlasThresholdAlert(deviceIdentity, ATLAS_THRESHOLD_ALERT_PROCS_PATH,
                                       ATLAS_THRESHOLD_ALERT_PROCS_RATE_SEC_DEFAULT,
                                       ATLAS_THRESHOLD_ALERT_PROCS_THRESHOLD_DEFAULT);
    
    return nullptr;
}

} // namespace atlas
