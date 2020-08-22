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
    * @return true if the command was handled without error, false otherwise
    */
    bool checkCommandPayload(const std::string &payload);

    /**
    * @brief Response with an ACK status to cloud back-end for a specific sequence number
    * @return true if the command was notified without error, false otherwise
    */
    bool responseCommandACK();

    /**
    * @brief Response with a DONE status to cloud back-end for a specific sequence number
    * @return true if the command was notified without error, false otherwise
    */
    bool responseCommandDONE();

    /**
    * @brief Set sequence number that must be notified as done to cloud
    * @param[in] sequenceNumberDONE Last executed device command
    * @return none
    */
    inline void setSequenceNumberDONE(const uint32_t sequenceNumber) { sequenceNumberDONE_ = sequenceNumber; }

    /**
    * @brief Get status of the scheduling window for DONE messages 
    * @return msgDONEScheduled_
    */
    inline bool getMsgDONEScheduled() const { return msgDONEScheduled_; }

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

    /**
     * @brief Cloud sync ACK status alarm callback
     * @return none
     */
    void statusACKCallback();

    /**
     * @brief Cloud sync DONE status alarm callback
     * @return none
     */
    void statusDONECallback();

    /* Push top-command alarm */
    AtlasAlarm pushCommandAlarm_;

    /* Cloud sync ACK status alarm */
    AtlasAlarm statusACKAlarm_;

    /* Cloud sync DONE status alarm */
    AtlasAlarm statusDONEAlarm_;

    /* Sequence number of the last command */
    uint32_t sequenceNumber_;

    /* Sequence number of the last executed device command */
    uint32_t sequenceNumberDONE_;

    /* status for ACK alarm scheduling */
    bool msgACKScheduled_;

    /* status for DONE alarm scheduling */
    bool msgDONEScheduled_;

    /* counter for resending ACK messages */
    uint8_t counterACK_;

    /* counter for resending DONE messages */
    uint8_t counterDONE_;
    
};

} // namespace atlas

#endif /* __ATLAS_APPROVE_H__ */
