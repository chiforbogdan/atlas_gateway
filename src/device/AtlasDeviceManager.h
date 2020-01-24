#ifndef __ATLAS_DEVICE_MANAGER_H__
#define __ATLAS_DEVICE_MANAGER_H__

#include <unordered_map>
#include "AtlasDevice.h"

namespace atlas {

class AtlasDeviceManager
{

public:
    /**
    * @brief Get device manager instance
    * @return device manager instance
    */
    static AtlasDeviceManager& getInstance();

    /**
    * @brief Get the client device associated with the given identity. If the client
    * device does not exist, then it will be created.
    * @param[in] identity Client device identity
    * @return Instance to client device
    */
    AtlasDevice& getDevice(const std::string& identity);

    AtlasDeviceManager(const AtlasDeviceManager&) = delete;
    AtlasDeviceManager& operator=(const AtlasDeviceManager&) = delete;

private:
    AtlasDeviceManager() = default;

    /* Client devices */
    std::unordered_map<std::string, AtlasDevice> devices_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_MANAGER_H__ */
