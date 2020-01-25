#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasDevice.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

/* Keep-alive counter initial value */
const int ATLAS_KEEP_ALIVE_COUNTER = 6;

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
