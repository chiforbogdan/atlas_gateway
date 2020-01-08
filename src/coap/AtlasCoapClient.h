#ifndef __ATLAS_COAP_CLIENT_H__
#define __ATLAS_COAP_CLIENT_H__

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <string>
#include <coap2/coap.h>
#include <functional>
#include "AtlasCoapMethod.h"
#include "AtlasCoapResponse.h"

namespace atlas {

typedef std::function<void(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)> coap_request_callback_t;

class AtlasCoapClient
{

public:

    /**
    * @brief Send CoAP client request
    * @param[in] uri CoAP URI
    * @param[in] method CoAP method
    * @param[in] reqPayload CoAP client request payload
    * @param[in] reqPayloadLen CoAP client request payload length
    * @param[in] timeout Timeout in ms
    * @param[in] callback CoAP request callback
    * @return none
    */
    void sendRequest(const std::string &uri, AtlasCoapMethod method, const uint8_t *reqPayload,
                     size_t reqPayloadLen, uint32_t timeout,
                     coap_request_callback_t callback);

    /**
    * @brief Set DTLS information
    * @param[in] identity DTLS identity
    * @param[in] DTLS Pre-shared key
    * @return none
    */
    void setDtlsInfo(const std::string &identity, const std::string &psk);

private:
    /**
    * @brief Resolve CoAP hostname
    * @param[in] hostname CoAP hostname
    * @param[out] dst Destination socket
    * @return Destination address length or -1 in case of error 
    */
    int resolveAddress(coap_str_const_t *hostname, struct sockaddr *dst);

    /**
    * @brief Create CoAP session
    * @param[in] ctx CoAP context
    * @param[in] proto CoAP protocol (UDP or DTLS)
    * @param[in] dst Destination address
    * @return CoAP session
    */
    coap_session_t *getSession(coap_context_t *ctx, coap_proto_t proto, coap_address_t *dst);

    /**
    * @brief Get CoAP request unique token
    * @return token
    */
    static uint32_t getToken();

    /* DTLS identity*/
    std::string dtlsIdentity_;

    /* DTLS key */
    std::string dtlsKey_;

    /* DTLS pre-shared key structure */
    coap_dtls_cpsk_t dtlsPsk_;

    /* CoAP client request callback */
    coap_request_callback_t callback_;

    /* CoAP request unique token */
    static uint32_t token_;
};

} // namespace atlas

#endif /* __ATLAS_COAP_CLIENT_H__ */
