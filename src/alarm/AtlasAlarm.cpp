#include <boost/bind.hpp>
#include "AtlasAlarm.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

AtlasAlarm::AtlasAlarm() : periodMs_(0), once_(true), callback_(nullptr), timer_(nullptr) {}

void AtlasAlarm::timerHandler(const boost::system::error_code& ec)
{
    ATLAS_LOGGER_DEBUG("Timer handler executed");

    if (!ec) {
        /* Start timer again */
        if (!once_) {
            timer_->expires_at(timer_->expires_at() + boost::posix_time::millisec(this->periodMs_));
            timer_->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1));
        } else
            cancel();
       
        this->callback_();

    } else if (ec == boost::asio::error::operation_aborted) {
        ATLAS_LOGGER_DEBUG("Timer aborted");
        
    } else {
        ATLAS_LOGGER_ERROR("Timer handler called with error message: " + ec.message());

    }
}

AtlasAlarm::AtlasAlarm(uint32_t periodMs, bool once, std::function<void()> callback) : timer_(nullptr)
{
    ATLAS_LOGGER_DEBUG("Alarm was created");

    periodMs_ = periodMs;
    once_ = once;
    callback_ = callback;
}

void AtlasAlarm::start()
{
    
    /* If timer is already started, there is nothing to do */
    if (timer_)
        return;

    timer_ = new boost::asio::deadline_timer(AtlasScheduler::getInstance().getService(),
                                             boost::posix_time::millisec(this->periodMs_));

    /* Start timer now */
    timer_->async_wait(boost::bind(&AtlasAlarm::timerHandler, this, _1));

    ATLAS_LOGGER_DEBUG("Alarm was started");
}

void AtlasAlarm::cancel()
{
    if (timer_) {
        ATLAS_LOGGER_DEBUG("Alarm was cancelled");
        timer_->cancel();
	    delete timer_;
        timer_ = nullptr;
    }
}

AtlasAlarm::~AtlasAlarm()
{
    cancel();
}

} // namespace atlas
