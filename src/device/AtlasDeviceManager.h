#ifndef __ATLAS_DEVICE_MANAGER_H__
#define __ATLAS_DEVICE_MANAGER_H__

#include <unordered_map>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include "AtlasDevice.h"
#include "../cloud/AtlasDeviceCloud.h"
#include "../telemetry/AtlasTelemetry.h"

namespace atlas {

class AtlasDeviceManager
{

public:
    /**
    * @brief Get device manager instance
    * @return Device manager instance
    */
    static AtlasDeviceManager& getInstance();

    /**
    * @brief Install all policies for all devices
    * @return none
    */
    void installAllPolicies();

    /**
    * @brief Get device cloud back-end manager
    * @return Device cloud back-end manager
    */
    std::shared_ptr<AtlasDeviceCloud> getDeviceCloud() { return deviceCloud_; }

    /**
    * @brief Get the client device associated with the given identity. If the client
    * device does not exist, then it will be created.
    * @param[in] identity Client device identity
    * @return Instance to client device
    */
    AtlasDevice& getDevice(const std::string& identity);

    /**
    * @brief Parse each device
    * @param[in] cb For each callback
    * @return none
    */
    void forEachDevice(std::function<void(AtlasDevice&)> cb);

    AtlasDeviceManager(const AtlasDeviceManager&) = delete;
    AtlasDeviceManager& operator=(const AtlasDeviceManager&) = delete;

private:
    /**
    * @brief Ctor for device manager
    * @return none
    */
    AtlasDeviceManager();
    
    /**
    * @brief Dtor for device manager
    * @return none
    */
    ~AtlasDeviceManager();

    /**
    * @brief Firewall-statistics alarm callback
    * @return none
    */
    void firewallStatisticsAlarmCallback();

    /* Client devices */
    std::unordered_map<std::string, AtlasDevice> devices_;

    /* Telemetry manager which handles the telemetry CoAP resources and pushes the data into the devices */
    AtlasTelemetry telemetry_;

    /* Cloud back-end manager */
    std::shared_ptr<AtlasDeviceCloud> deviceCloud_;

    /* Firewall-statistics alarm */
    AtlasAlarm fsAlarm_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_MANAGER_H__ */
