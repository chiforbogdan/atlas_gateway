#include "AtlasAlertFactory.h"
#include "AtlasTelemetryInfo.h"

namespace atlas {

/* Sysinfo procs default telemetry alert values */
const uint16_t ATLAS_ALERT_PROCS_EXT_PUSH_RATE_SEC_DEFAULT = 60;
const uint16_t ATLAS_ALERT_PROCS_INT_SCAN_RATE_SEC_DEFAULT = 30;
const std::string ATLAS_ALERT_PROCS_THRESHOLD_DEFAULT = "1000";
const std::string ATLAS_ALERT_PROCS_PATH = "client/telemetry/alerts/sysinfo/procs";

AtlasAlert* AtlasAlertFactory::getAlert(const std::string& type, const std::string &deviceIdentity)
{
    if (type == TELEMETRY_SYSINFO_PROCS)
        return new AtlasAlert(deviceIdentity, ATLAS_ALERT_PROCS_PATH, ATLAS_ALERT_PROCS_EXT_PUSH_RATE_SEC_DEFAULT,
                              ATLAS_ALERT_PROCS_INT_SCAN_RATE_SEC_DEFAULT, ATLAS_ALERT_PROCS_THRESHOLD_DEFAULT);
    
    return nullptr;
}

} // namespace atlas
