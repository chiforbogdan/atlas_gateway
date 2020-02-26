#include "AtlasAlertFactory.h"
#include "AtlasTelemetryInfo.h"

namespace atlas {

namespace {
/* Sysinfo procs default push alert values */
const uint16_t ATLAS_PUSH_ALERT_PROCS_RATE_SEC_DEFAULT = 60;
const std::string ATLAS_PUSH_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/push";

/* Packets per minute default push alert values */
const uint16_t ATLAS_PUSH_ALERT_PACKETS_PER_MINUTE_RATE_SEC_DEFAULT = 60;
const std::string ATLAS_PUSH_ALERT_PACKETS_PER_MIN_PATH = "client/telemetry/packets_info/packets_per_min/alerts/push";

/* Packets average length default push alert values */
const uint16_t ATLAS_PUSH_ALERT_PACKETS_AVG_RATE_SEC_DEFAULT = 60;
const std::string ATLAS_PUSH_ALERT_PACKETS_AVG_PATH = "client/telemetry/packets_info/packets_avg/alerts/push";

/* Sysinfo procs default threshold alert value */
const uint16_t ATLAS_THRESHOLD_ALERT_PROCS_RATE_SEC_DEFAULT = 2;
const std::string ATLAS_THRESHOLD_ALERT_PROCS_THRESHOLD_DEFAULT = "1000";
const std::string ATLAS_THRESHOLD_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/threshold";

/* Packets per minute default threshold alert value */
const uint16_t ATLAS_THRESHOLD_ALERT_PACKETS_PER_MINUTE_RATE_SEC_DEFAULT = 5;
const std::string ATLAS_THRESHOLD_ALERT_PACKETS_PER_MINUTE_THRESHOLD_DEFAULT = "100";
const std::string ATLAS_THRESHOLD_ALERT_PACKETS_PER_MINUTE_PATH = "client/telemetry/packets_info/packets_per_min/alerts/threshold";

/* Packets average length default threshold alert value */
const uint16_t ATLAS_THRESHOLD_ALERT_PACKETS_AVG_RATE_SEC_DEFAULT = 3;
const std::string ATLAS_THRESHOLD_ALERT_PACKETS_AVG_THRESHOLD_DEFAULT = "1000";
const std::string ATLAS_THRESHOLD_ALERT_PACKETS_AVG_PATH = "client/telemetry/packets_info/packets_avg/alerts/threshold";

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
    else if (type == TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_PACKETS_PER_MIN_PATH,
                                  ATLAS_PUSH_ALERT_PACKETS_PER_MINUTE_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_PACKETS_INFO_PACKETS_AVG)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_PACKETS_AVG_PATH,
                                  ATLAS_PUSH_ALERT_PACKETS_AVG_RATE_SEC_DEFAULT);
    
    return nullptr;
}

AtlasThresholdAlert* AtlasAlertFactory::getThresholdAlert(const std::string& type, const std::string &deviceIdentity)
{
    if (type == TELEMETRY_SYSINFO_PROCS)
        return new AtlasThresholdAlert(deviceIdentity, ATLAS_THRESHOLD_ALERT_PROCS_PATH,
                                       ATLAS_THRESHOLD_ALERT_PROCS_RATE_SEC_DEFAULT,
                                       ATLAS_THRESHOLD_ALERT_PROCS_THRESHOLD_DEFAULT);
    else if (type == TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE)
        return new AtlasThresholdAlert(deviceIdentity, ATLAS_THRESHOLD_ALERT_PACKETS_PER_MINUTE_PATH,
                                       ATLAS_THRESHOLD_ALERT_PACKETS_PER_MINUTE_RATE_SEC_DEFAULT,
                                       ATLAS_THRESHOLD_ALERT_PACKETS_PER_MINUTE_THRESHOLD_DEFAULT);
    else if (type == TELEMETRY_PACKETS_INFO_PACKETS_AVG)
        return new AtlasThresholdAlert(deviceIdentity, ATLAS_THRESHOLD_ALERT_PACKETS_AVG_PATH,
                                       ATLAS_THRESHOLD_ALERT_PACKETS_AVG_RATE_SEC_DEFAULT,
                                       ATLAS_THRESHOLD_ALERT_PACKETS_AVG_THRESHOLD_DEFAULT);
    
    return nullptr;
}

} // namespace atlas
