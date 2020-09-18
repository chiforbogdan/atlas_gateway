#ifndef __ATLAS_COMMAND_DEVICE_H__
#define __ATLAS_COMMAND_DEVICE_H__

#include <stdint.h>
#include <string>


namespace atlas {

class AtlasCommandDevice
{

public:
    /**
    * @brief Ctor for command
    * @param[in] deviceIdentity Device identity
    * @param[in] sequenceNumber Command sequence number
    * @param[in] commandType Command type
    * @param[in] commandPayload Command payload
    * @return none
    */
    AtlasCommandDevice(const std::string &deviceIdentity, const uint32_t sequenceNumber, 
                       const std::string &commandType, const std::string &commandPayload);

    /**
    * @brief Used in priority Q tree
    *        Top will contains the smallest sequence number value
    */
    inline bool operator<(const AtlasCommandDevice& c) const { return sequenceNumber_ > c.sequenceNumber_; }

    /**
    * @brief Get command device sequence number
    * @return sequenceNumber_
    */
    inline uint32_t getSequenceNumber() const { return sequenceNumber_; }

    /**
     * @brief Get device identity
     * @return Device identity
     */
    inline std::string getDeviceIdentity() const { return deviceIdentity_; }

    /**
     * @brief Get command type for client
     * @return command type device
     */
    inline uint16_t getCommandTypeDevice() const { return commandTypeDevice_; }

private:
    /* Client device identity */
    std::string deviceIdentity_;
    /* Command sequence number */
    uint32_t sequenceNumber_;
    /* Command type from cloud*/
    std::string commandTypeCloud_;
    /* Command type to device*/
    uint16_t commandTypeDevice_;
    /* Command payload */
    std::string commandPayload_;
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_DEVICE_H__ */
