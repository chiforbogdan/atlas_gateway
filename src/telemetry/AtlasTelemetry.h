#ifndef __ATLAS_TELEMETRY_H__
#define __ATLAS_TELEMETRY_H__

#include "../coap/AtlasCoapResponse.h"
#include "../coap/AtlasCoapMethod.h"
#include "../coap/AtlasCoapResource.h"

namespace atlas {

class AtlasTelemetry {

public:
    /**
    * @brief Ctor for telemetry
    * @return none
    */
    AtlasTelemetry();

private:
    /**
    * @brief Hostname telemetry client callback
    * @param[in] path CoAP URI path
    * @param[in] pskIdentity PSK identity (extracted from the DTLS transport layer)
    * @param[in] psk Pre-shared key
    * @param[in] method CoAP method
    * @param[in] reqPayload Request payload
    * @param[in] reqPayloadLen Request payload length
    * @param[out] respPayload Response payload
    * @param[out] respPayloadLen Response payload length
    * @return CoAP response status
    */
    AtlasCoapResponse hostnameCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                       AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                       uint8_t **respPayload, size_t *respPayloadLen);

    /**
    * @brief Kernel info telemetry client callback
    * @param[in] path CoAP URI path
    * @param[in] pskIdentity PSK identity (extracted from the DTLS transport layer)
    * @param[in] psk Pre-shared key
    * @param[in] method CoAP method
    * @param[in] reqPayload Request payload
    * @param[in] reqPayloadLen Request payload length
    * @param[out] respPayload Response payload
    * @param[out] respPayloadLen Response payload length
    * @return CoAP response status
    */
    AtlasCoapResponse kernInfoCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                       AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                        uint8_t **respPayload, size_t *respPayloadLen);



    /* Hostname telemetry CoAP resource*/
    AtlasCoapResource hostnameResource_;

    /* Kernel info telemetry CoAP resource*/
    AtlasCoapResource kernInfoResource_;
};

} // namespace atlas

#endif /* __ATLAS_TELEMETRY_H__ */
