#ifndef __ATLAS_COAP_SERVER_H__
#define __ATLAS_COAP_SERVER_H__

#include <coap2/coap.h>
#include <string>
#include <unordered_map>
#include "AtlasCoapResource.h"

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

    /**
     * @brief Instance getter for CoAP server
     * @return CoAP server instance
     */
    static AtlasCoapServer& getInstance();

    /**
     * @brief Start CoAP server
     * @param[in] hostname CoAP hostname
     * @param[in] port CoAP port
     * @param[in] mode Server mode (UDP, DTLS or both)
     * @param[in] psk Pre-shared key for DTLS mode
     */
    void start(const std::string &hostname, const std::string &port,
                    AtlasCoapServerMode mode, const std::string &psk);


    /**
     * @brief Add CoAP resource
     * @param[in] resource CoAP resource
     * @return none
     */
    void addResource(const AtlasCoapResource &resource);
    
    /**
     * @brief Delete CoAP resource
     * @param[in] resource CoAP resource
     * @return none
     */
    void delResource(const AtlasCoapResource &resource);

    AtlasCoapServer(const AtlasCoapServer&) = delete;
    AtlasCoapServer &operator=(const AtlasCoapServer& ) = delete;

private:

    AtlasCoapServer();

    coap_context_t *getContext(const std::string &hostname, const std::string &port,
                                AtlasCoapServerMode mode, const std::string &psk);
    
    void setDtlsPsk(coap_context_t *ctx, const std::string &pskVal);

    void initDefaultResources(coap_context_t *ctx);

    static void getDefaultIndexHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session,
                                coap_pdu_t *request, coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    static void getHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    static void postHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    static void putHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    static void deleteHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    void incomingHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                         coap_binary_t *token, coap_string_t *query, coap_pdu_t *response, AtlasCoapMethod method);

    void schedulerCallback(); 

    coap_context_t *ctx_;

    uint8_t key_[ATLAS_MAX_PSK_KEY_BYTES + 1];

    std::unordered_map<std::string, AtlasCoapResource> resources_;
};

} //namespace atlas

#endif /* __ATLAS_COAP_SERVER_H__ */
