#ifndef __ATLAS_COAP_SERVER_H__
#define __ATLAS_COAP_SERVER_H__

#include <coap2/coap.h>
#include <string>

#define ATLAS_MAX_PSK_KEY_BYTES (64)

namespace atlas {

enum AtlasCoapServerMode {
    /* UDP transport mode */
    ATLAS_COAP_SERVER_MODE_UDP = (1 << 0),
 
    /* DTLS with PSK transport mode */
    ATLAS_COAP_SERVER_MODE_DTLS_PSK = (1 << 1),
 
    /* UDP + DTLS PSK transport modes */
    ATLAS_COAP_SERVER_MODE_BOTH = (ATLAS_COAP_SERVER_MODE_UDP | ATLAS_COAP_SERVER_MODE_DTLS_PSK),
};

class AtlasCoapServer {

public:
    AtlasCoapServer(const std::string &hostname, const std::string &port,
                    AtlasCoapServerMode mode, const std::string &psk);

private:
    coap_context_t *getContext(const std::string &hostname, const std::string &port,
                                AtlasCoapServerMode mode, const std::string &psk);
    
    void setDtlsPsk(coap_context_t *ctx, const std::string &pskVal);

    void initDefaultResources(coap_context_t *ctx);

    static void getDefaultIndexHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session,
                                coap_pdu_t *request, coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    void schedulerCallback(); 

    coap_context_t *ctx_;

    uint8_t key_[ATLAS_MAX_PSK_KEY_BYTES + 1];

};

} //namespace atlas

#endif /* __ATLAS_COAP_SERVER_H__ */
