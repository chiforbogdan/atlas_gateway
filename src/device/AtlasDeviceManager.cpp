#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasDeviceManager.h"
#include "../logger/AtlasLogger.h"
#include "../scheduler/AtlasScheduler.h"

namespace atlas {

const int ATLAS_KEEP_ALIVE_TIME_SEC = 20;

AtlasDeviceManager& AtlasDeviceManager::getInstance()
{
    static AtlasDeviceManager instance;

    return instance;
}

AtlasDeviceManager::AtlasDeviceManager() : kaTimer_(AtlasScheduler::getInstance().getService(),
                                                    boost::posix_time::seconds(ATLAS_KEEP_ALIVE_TIME_SEC)) 
{
    kaTimer_.async_wait(boost::bind(&AtlasDeviceManager::kaTimerHandler, this, _1));
}

void AtlasDeviceManager::kaTimerHandler(const boost::system::error_code& ec)
{
    ATLAS_LOGGER_DEBUG("Device manager keep-alive timer handler executed");
     
    if (!ec) {
        /* Start timer again */
        kaTimer_.expires_at(kaTimer_.expires_at() + boost::posix_time::seconds(ATLAS_KEEP_ALIVE_TIME_SEC));
        kaTimer_.async_wait(boost::bind(&AtlasDeviceManager::kaTimerHandler, this, _1));

        /* Notify all devices about the keep-alive timer */
        std::unordered_map<std::string, AtlasDevice>::iterator it = devices_.begin();
        while (it != devices_.end()) {
            (*it).second.keepAliveExpired();
            ++it;
        }
    } else
        ATLAS_LOGGER_ERROR("Device manager keep-alive timer handler called with error");
}

AtlasDevice& AtlasDeviceManager::getDevice(const std::string& identity)
{
    AtlasDevice& device = devices_[identity];

    if (device.getIdentity() == "") {
        device.setIdentity(identity);
        ATLAS_LOGGER_INFO1("New client device created with identity ", identity);
    }

    return device;
}

} // namespace atlas
