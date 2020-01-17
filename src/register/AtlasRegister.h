#ifndef __ATLAS_REGISTER_H__
#define __ATLAS_REGISTER_H__

#include "../coap/AtlasCoapResponse.h"
#include "../coap/AtlasCoapMethod.h"
#include "../coap/AtlasCoapResource.h"

namespace atlas {

class AtlasRegister
{

public:
    AtlasRegister();

    void start();
    void stop();

private:
    /**
    * @brief Register client callback
    * @param[in] path CoAP URI path
    * @param[in] method CoAP method
    * @param[in] reqPayload Request payload
    * @param[in] reqPayloadLen Request payload length
    * @param[out] respPayload Response payload
    * @param[out] respPayloadLen Response payload length
    * @return CoAP response status
    */
    AtlasCoapResponse registerCallback(std::string path, AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen, uint8_t **respPayload, size_t *respPayloadLen);
    
    /**
    * @brief Keepalive client callback
    * @param[in] path CoAP URI path
    * @param[in] method CoAP method
    * @param[in] reqPayload Request payload
    * @param[in] reqPayloadLen Request payload length
    * @param[out] respPayload Response payload
    * @param[out] respPayloadLen Response payload length
    * @return CoAP response status
    */
    AtlasCoapResponse keepaliveCallback(std::string path, AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen, uint8_t **respPayload, size_t *respPayloadLen);


    AtlasCoapResource registerResource_;
    AtlasCoapResource keepAliveResource_;
};

} // namespace atlas

#endif /* __ATLAS_REGISTER_H__ */
