#ifndef __ATLAS_ALARM_H__
#define __ATLAS_ALARM_H__

#include <stdint.h>
#include <functional>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace atlas {

class AtlasAlarm
{

public:
    /**
    * @brief Ctor for alarm
    * @return none
    */
    AtlasAlarm();

    /**
     * @brief Ctor for alarm
     * @param[in] periodMs Timer period in ms
     * @param[in] once Indicates if the timer should run only once
     * @param[in] callback Timer callback
     * @return none
     */
    AtlasAlarm(uint32_t periodMs, bool once, std::function<void()> callback);
    
    /**
     * @brief Dtor for alarm. It cancels any active timers.
     * @return none
     */
    ~AtlasAlarm();

    /**
     * @brief Start an alarm
     * @return none
     */
    void start();

    /**
     * @brief Cancel a started alarm
     * @return none
     */
    void cancel();

private:
    /**
     * @brief Internal timer handler
     * @param[in] ec Error code
     * @return none
     */
    void timerHandler(const boost::system::error_code& ec);
    
    /* Timer period in ms */
    uint32_t periodMs_;
    
    /* Indicates if the timer should run only once */
    bool once_;
    
    /* High layer application callback */
    std::function<void()> callback_;
    
    /* Timer object */
    boost::asio::deadline_timer *timer_; 
};

} // namespace atlas

#endif /* __ATLAS_ALARM_H__ */
