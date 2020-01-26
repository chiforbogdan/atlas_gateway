#ifndef __ATLAS_ALERT_FACTORY_H__
#define __ATLAS_ALERT_FACTORY_H__

#include "AtlasAlert.h"

namespace atlas {

class AtlasAlertFactory
{
public:
    /**
    * @brief Create telemetry alert
    * @param[in] type Telemetry feature type
    * @param[in] deviceIdentity Client device identity
    * @return Telemetry alert
    */
    static AtlasAlert* getAlert(const std::string& type, const std::string &deviceIdentity_);
};

} // namespace atlas

#endif /* __ATLAS_ALERT_FACTORY_H__ */
