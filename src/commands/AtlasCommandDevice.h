#ifndef __ATLAS_COMMAND_DEVICE_H__
#define __ATLAS_COMMAND_DEVICE_H__

#include <stdint.h>
#include <string>
#include "../coap/AtlasCoapResponse.h"

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
    * @brief Push command to device
    * @return none
    */
    void pushCommand();

    /**
    * @brief Dtor for command
    * @return none
    */
    ~AtlasCommandDevice();

private:

    /**
    * @brief CoAP response callback
    * @param[in] respStatus CoAP response status
    * @param[in] resp_payload CoAP response payload
    * @param[in] resp_payload_len CoAP response payload length
    * @return none
    */
    void respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len);
   
    /* CoAP context*/
    void *coapToken_;
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
