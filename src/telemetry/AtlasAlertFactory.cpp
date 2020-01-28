#include "AtlasAlertFactory.h"
#include "AtlasTelemetryInfo.h"

namespace atlas {

/* Sysinfo procs default telemetry alert values */
const uint16_t ATLAS_PUSH_ALERT_PROCS_RATE_SEC_DEFAULT = 60;
const uint16_t ATLAS_THRESHOLD_ALERT_PROCS_RATE_SEC_DEFAULT = 30;
const std::string ATLAS_THRESHOLD_ALERT_PROCS_THRESHOLD_DEFAULT = "1000";
const std::string ATLAS_PUSH_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/push";
const std::string ATLAS_THRESHOLD_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/threshold";

AtlasPushAlert* AtlasAlertFactory::getPushAlert(const std::string& type, const std::string &deviceIdentity)
{
    if (type == TELEMETRY_SYSINFO_PROCS)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_PROCS_PATH,
                                  ATLAS_PUSH_ALERT_PROCS_RATE_SEC_DEFAULT);
    
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
