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
    * @param[in] pskIdentity PSK identity (extracted from the DTLS transport layer)
    * @param[in] method CoAP method
    * @param[in] reqPayload Request payload
    * @param[in] reqPayloadLen Request payload length
    * @param[out] respPayload Response payload
    * @param[out] respPayloadLen Response payload length
    * @return CoAP response status
    */
    AtlasCoapResponse registerCallback(const std::string &path, const std::string &pskIdentity,
                                       AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                       uint8_t **respPayload, size_t *respPayloadLen);
    
    /**
    * @brief Keepalive client callback
    * @param[in] path CoAP URI path
    * @param[in] pskIdentity PSK identity (extracted from the DTLS transport layer)
    * @param[in] method CoAP method
    * @param[in] reqPayload Request payload
    * @param[in] reqPayloadLen Request payload length
    * @param[out] respPayload Response payload
    * @param[out] respPayloadLen Response payload length
    * @return CoAP response status
    */
    AtlasCoapResponse keepaliveCallback(const std::string &path, const std::string &identity,
                                        AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                        uint8_t **respPayload, size_t *respPayloadLen);

    /* REGISTER command CoAP resource*/
    AtlasCoapResource registerResource_;

    /* KEEPALIVE command CoAP resource*/
    AtlasCoapResource keepAliveResource_;
};

} // namespace atlas

#endif /* __ATLAS_REGISTER_H__ */
