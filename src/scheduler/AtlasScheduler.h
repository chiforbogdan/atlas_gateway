#ifndef __ATLAS_SCHEDULER_H__
#define __ATLAS_SCHEDULER_H__

#include <unordered_map>
#include <boost/asio.hpp>
#include "AtlasSchedulerFdEntry.h"

namespace atlas {

class AtlasScheduler {

public:

    /**
    * @brief Ctor for scheduler
    * @return none
    */
    AtlasScheduler();

    /*
    * @brief Run scheduler main loop
    * @return none
    */
    void run();
    
    /*
    * @brief Add file descriptor entry to scheduler
    * @param[in] fd File descriptor
    * @param[in] cb callback
    * @return none
    */
    void addFdEntry(int fd, std::function<void()> cb);

    /**
    * @brief Delete file descriptor entry from scheduler
    * @param[in] fd File descriptor
    * @return none
    */ 
    void delFdEntry(int fd);

private:
    /* Map to store the file descriptor entries */
    std::unordered_map<int, std::unique_ptr<AtlasSchedulerFdEntry>> entries_;
    
    /* Boost io_service */
    boost::asio::io_service ioService_;
    
    /* Boost io_service work (main loop) */
    boost::asio::io_service::work work_;
};

} // namespace atlas

#endif /* __ATLAS_SCHEDULER_H__ */
