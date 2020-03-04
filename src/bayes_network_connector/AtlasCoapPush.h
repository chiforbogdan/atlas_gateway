#ifndef __ATLAS_COAP_PUSH_H__
#define __ATLAS_COAP_PUSH_H__

#include <string>
#include "../coap/AtlasCoapResponse.h"

namespace atlas {

const uint16_t ATLAS_COAP_TIMEOUT_MS = 5000;

class AtlasCoapPush
{
public:
    /**
    * @brief Ctor for feature reputation
    * @param[in] deviceIdentity Client device identity
    * @param[in] path Feature reputation CoAP path
    * @return none
    */
    AtlasCoapPush(const std::string deviceIdentity);
    
    /**
    * @brief Push feature reputation to client device
    * @return none
    */
    void push();

    /**
    * @brief Dtor for feature reputation
    * @return none
    */
    virtual ~AtlasCoapPush() {}
protected:
     /**
    * @brief CoAP response callback
    * @param[in] respStatus CoAP response status
    * @param[in] resp_payload CoAP response payload
    * @param[in] resp_payload_len CoAP response payload length
    * @return none
    */
    void respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len);
   
    void pushCommand(const std::string &path);

private:

    /* Client device identity */
    std::string deviceIdentity_;

    /* Feature reputation CoAP path */
    std::string path_;
};

} // namespace atlas

#endif /* __ATLAS_COAP_PUSH_H__ */
