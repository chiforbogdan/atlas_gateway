#ifndef __ATLAS_ALERT_H__
#define __ATLAS_ALERT_H__

#include <string>
#include "../coap/AtlasCoapResponse.h"

namespace atlas {

class AtlasAlert
{
public:
    /**
    * @brief Ctor for telemetry alert
    * @param[in] deviceIdentity Client device identity
    * @param[in] path Telemetry alert CoAP path
    * @param[in] extPushRate External push rate value
    * @param[in] intScanRate Internal scan rate value
    * @param[in] threshold Threshold value
    * @return none
    */
    AtlasAlert(const std::string &deviceIdentity, const std::string &path,
               uint16_t extPushRate, uint16_t intScanRate,
               const std::string &threshold);
    
    /**
    * @brief Push telemetry alert to client device
    * @return none
    */
    void push();

private:

    /**
    * @brief CoAP response callbacl
    * @param[in] respStatus CoAP response status
    * @param[in] resp_payload CoAP response payload
    * @param[in] resp_payload_len CoAP response payload length
    * @return none
    */
    void respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len);

    /* Client device identity */
    std::string deviceIdentity_;

    /* Telemetry alert CoAP path */
    std::string path_;

    /* Telemetry alert external push to gateway rate */
    uint16_t extPushRate_;

    /* Telemetry alert internal scan rate */
    uint16_t intScanRate_;

    /* Telemetry alert threshold (string because it is application specific) */
    std::string threshold_;
};

} // namespace atlas

#endif /* __ATLAS_TELEMETRY_ALERT_H__ */
