#ifndef __ATLAS_APPROVE_H__
#define __ATLAS_APPROVE_H__

#include <string>

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
    * @brief Check approved cmd payload received from cloud back-end
    * @return none
    */
    void checkCmdPayload(const std::string &cmdPayload);

    /**
    * @brief Relay approved cmd to targeted device
    * @return none
    */
    void RelayCmd(std::string const &cmd);

    AtlasApprove(const AtlasApprove &) = delete;
    AtlasApprove& operator=(const AtlasApprove &) = delete;

private:
    /**
    * @brief Default ctor for device approve module
    * @return none
    */
    AtlasApprove();

    /* Sequence number of the last authenticated command */
    static uint32_t sequenceNumber_;
};

} // namespace atlas

#endif /* __ATLAS_APPROVE_H__ */
