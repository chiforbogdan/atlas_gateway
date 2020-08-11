#ifndef __ATLAS_APPROVE_H__
#define __ATLAS_APPROVE_H__

#include <string>
#include "../alarm/AtlasAlarm.h"

namespace atlas {

class AtlasApprove
{
public:
    /**
    * @brief Get instance for device approve module
    * @return Device approve module instance
    */
    static AtlasApprove& getInstance();

    /**
     * @brief Start approve protocol
     * @return none
     */
    void start();

    /**
     * @brief Stop approve protocol
     * @return none
     */
    void stop();

    /**
    * @brief Check approved cmd payload received from cloud back-end
    * @param[in] payload Command payload
    * @return none
    */
    void checkCommandPayload(const std::string &payload);

    /**
    * @brief Response with an ACK status to cloud back-end for a specific sequence number
    * @param[in] sequenceNumber Command sequnce number
    * @return none
    */
    void ResponseCommandACK(const uint32_t sequenceNumber);

    /**
    * @brief Response with a DONE status to cloud back-end for a specific sequence number
    * @param[in] sequenceNumber Command sequnce number
    * @return none
    */
    void ResponseCommandDONE(const uint32_t sequenceNumber);

    AtlasApprove(const AtlasApprove &) = delete;
    AtlasApprove& operator=(const AtlasApprove &) = delete;

private:
    /**
    * @brief Default ctor for device approve module
    * @return none
    */
    AtlasApprove();

    /**
     * @brief Push top-command alarm callback
     * @return none
     */
    void alarmCallback();

    /* Push top-command alarm */
    AtlasAlarm pushCommandAlarm_;

    /* Sequence number of the last command */
    static uint32_t sequenceNumber_;
    
};

} // namespace atlas

#endif /* __ATLAS_APPROVE_H__ */
