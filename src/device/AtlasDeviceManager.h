#ifndef __ATLAS_DEVICE_MANAGER_H__
#define __ATLAS_DEVICE_MANAGER_H__

#include <unordered_map>
#include <boost/asio.hpp>
#include "AtlasDevice.h"

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
    * @brief Get the client device associated with the given identity. If the client
    * device does not exist, then it will be created.
    * @param[in] identity Client device identity
    * @return Instance to client device
    */
    AtlasDevice& getDevice(const std::string& identity);

    AtlasDeviceManager(const AtlasDeviceManager&) = delete;
    AtlasDeviceManager& operator=(const AtlasDeviceManager&) = delete;

private:
    /**
    * @brief Ctor for device manager
    * @return none
    */
    AtlasDeviceManager();

    /**
    * @brief Internal keep-alive timer handler
    * @param[in] ec Error code
    * @return none
    */
    void kaTimerHandler(const boost::system::error_code& ec);

    /* Client devices */
    std::unordered_map<std::string, AtlasDevice> devices_;

    /* Keep-alive timer  */
    boost::asio::deadline_timer kaTimer_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_MANAGER_H__ */
