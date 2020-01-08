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
        if (!this->once_) {
            this->timer_->expires_at(timer_->expires_at() + boost::posix_time::millisec(this->periodMs_));
            this->timer_->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1));
        } else
            this->cancel();
    } else
        ATLAS_LOGGER_ERROR("Timer handler called with error");
        
}

AtlasAlarm::AtlasAlarm(uint32_t periodMs, bool once, std::function<void()> callback)
{
    this->periodMs_ = periodMs;
    this->once_ = once;
    this->callback_ = callback;
}

void AtlasAlarm::start()
{
    
    /* If timer is already started, there is nothing to do */
    if (this->timer_)
        return;

    timer_ = AtlasScheduler::getInstance().getTimer(this->periodMs_);

    /* Start timer now */
    timer_->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1));
}

void AtlasAlarm::cancel()
{
    if (this->timer_) {
        timer_->cancel();
	delete timer_;
    }
}

AtlasAlarm::~AtlasAlarm()
{
    this->cancel();
}

} // namespace atlas
