#include <boost/bind.hpp>
#include "AtlasAlarm.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

void AtlasAlarm::timerHandler(const boost::system::error_code& ec)
{
    ATLAS_LOGGER_DEBUG("Timer handler executed");

    if (!ec) {
        this->callback_();

        /* Start timer again */
        if (!once_) {
            timer_->expires_at(timer_->expires_at() + boost::posix_time::millisec(this->periodMs_));
            timer_->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1));
        } else
            cancel();
    } else
        ATLAS_LOGGER_ERROR("Timer handler called with error");
        
}

AtlasAlarm::AtlasAlarm(uint32_t periodMs, bool once, std::function<void()> callback)
{
    periodMs_ = periodMs;
    once_ = once;
    callback_ = callback;
}

void AtlasAlarm::start()
{
    
    /* If timer is already started, there is nothing to do */
    if (timer_)
        return;

    timer_ = AtlasScheduler::getInstance().getTimer(this->periodMs_);

    /* Start timer now */
    timer_->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1));
}

void AtlasAlarm::cancel()
{
    if (timer_) {
        timer_->cancel();
	delete timer_;
    }
}

AtlasAlarm::~AtlasAlarm()
{
    cancel();
}

} // namespace atlas
