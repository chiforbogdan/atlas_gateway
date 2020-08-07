#ifndef __ATLAS_COMMAND_CLIENT_H__
#define __ATLAS_COMMAND_CLIENT_H__

#include <stdint.h>

namespace atlas {


class AtlasCommandClient
{

public:
    /**
    * @brief Ctor for command
    * @param[in] sequenceNumber Command sequence number
    * @param[in] commandType Command type
    * @param[in] commandPayload Command payload
    * @return none
    */
    AtlasCommandClient(uint32_t sequenceNumber, const std::string &commandType, const std::string &commandPayload) : sequenceNumber_(sequenceNumber), commandType_(commandType), commandPayload_(commandPayload) {}
    
    /**
    * @brief Getter for command type
    * @return command type
    */
    std::string getType() const { return commandType_; }
    
    /**
    * @brief Getter for command payload
    * @return command payload
    */
    std::string getPayload() const { return commandPayload_; }
    
    /**
    * @brief Getter for command sequence number
    * @return command sequence number
    */
    uint32_t getSequenceNumber() const { return sequenceNumber_; }

    /**
    * @brief Used in priority Q tree
    *        Top will contains the smallest sequence number value
    */
    inline bool operator<(const AtlasCommandClient& c) const { return sequenceNumber_ > c.sequenceNumber_; }

private:
    /* Command sequence number */
    uint32_t sequenceNumber_;
    /* Command type */
    std::string commandType_;
    /* Command payload */
    std::string commandPayload_;
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_CLIENT_H__ */
