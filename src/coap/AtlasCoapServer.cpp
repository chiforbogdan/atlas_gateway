#include <iostream>
#include <coap2/coap.h>
#include <exception>
#include <coap2/coap.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "AtlasCoapServer.h"
#include "AtlasCoapException.h"
#include "../scheduler/AtlasScheduler.h"

#define ATLAS_COAP_SERVER_IS_UDP(MODE) ((MODE) & ATLAS_COAP_SERVER_MODE_UDP)
#define ATLAS_COAP_SERVER_IS_DTLS(MODE) ((MODE) & ATLAS_COAP_SERVER_MODE_DTLS_PSK)

namespace atlas {

const std::string DEFAULT_INDEX = "ATLAS CoAP gateway";

void AtlasCoapServer::schedulerCallback()
{
    coap_run_once(ctx_, COAP_RUN_NONBLOCK);
}

void AtlasCoapServer::getDefaultIndexHandler(coap_context_t *ctx,
                                             struct coap_resource_t *resource,
                                             coap_session_t *session,
                                             coap_pdu_t *request,
                                             coap_binary_t *token,
                                             coap_string_t *query,
                                             coap_pdu_t *response) {

    const char *INDEX = DEFAULT_INDEX.c_str();

    coap_add_data_blocked_response(resource, session, request, response, token,
                                   COAP_MEDIATYPE_TEXT_PLAIN, 0x2ffff,
                                   strlen(INDEX),
                                   (const uint8_t *)INDEX);
}

void AtlasCoapServer::initDefaultResources(coap_context_t *ctx)
{
    coap_resource_t *resource;

    resource = coap_resource_init(NULL, 0);
    coap_register_handler(resource, COAP_REQUEST_GET, &AtlasCoapServer::getDefaultIndexHandler);
    coap_add_resource(ctx, resource);
}

void AtlasCoapServer::setDtlsPsk(coap_context_t *ctx, const std::string &pskVal)
{
    coap_dtls_spsk_t psk;

    if (!coap_dtls_is_supported())
        throw AtlasCoapException("CoAP DTLS is not supported");
 
    memset(&psk, 0, sizeof(psk));
    psk.version = COAP_DTLS_SPSK_SETUP_VERSION;
 
    strncpy((char *) key_, pskVal.c_str(), ATLAS_MAX_PSK_KEY_BYTES);
    psk.psk_info.key.s = key_;
    psk.psk_info.key.length = strlen((char *) key_);
 
    /* Set PSK */
    coap_context_set_psk2(ctx, &psk);
 }


coap_context_t* AtlasCoapServer::getContext(const std::string &hostname, const std::string &port,
            AtlasCoapServerMode mode, const std::string &psk)
{
    struct addrinfo hints;
    struct addrinfo *res, *r;
    coap_context_t *ctx = NULL;
    coap_address_t addr, addrs;
    uint16_t tmp;
    coap_endpoint_t *ep, *eps;
 
    ctx = coap_new_context(NULL);
    if (!ctx)
        return NULL;
 
    /* Set DTLS PSK */
    if (ATLAS_COAP_SERVER_IS_DTLS(mode))
        this->setDtlsPsk(ctx, psk);
 
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    /* Use UDP */
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
 
    if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res) != 0) {
        //ATLAS_LOGGER_ERROR("Cannot start CoAP server");
        coap_free_context(ctx);
        return NULL;
    }

    for (r = res; r; r = r->ai_next) {
        if (r->ai_addrlen > sizeof(addr.addr))
            continue;

        coap_address_init(&addr);
        addr.size = r->ai_addrlen;
        memcpy(&addr.addr, r->ai_addr, addr.size);
        addrs = addr;

        if (ATLAS_COAP_SERVER_IS_UDP(mode) && ATLAS_COAP_SERVER_IS_DTLS(mode)) {
            if (addr.addr.sa.sa_family == AF_INET) {
                tmp = ntohs(addr.addr.sin.sin_port) + 1;
                addrs.addr.sin.sin_port = htons(tmp);
            } else if (addr.addr.sa.sa_family == AF_INET6) {
                uint16_t temp = ntohs(addr.addr.sin6.sin6_port) + 1;
                addrs.addr.sin6.sin6_port = htons(temp);
            }
        }

        if (ATLAS_COAP_SERVER_IS_UDP(mode)) {
            ep = coap_new_endpoint(ctx, &addr, COAP_PROTO_UDP);
            if (!ep) {
                //ATLAS_LOGGER_ERROR("Cannot open COAP UDP");
                continue;
            }
        }

        if (ATLAS_COAP_SERVER_IS_DTLS(mode)) {
            eps = coap_new_endpoint(ctx, &addrs, COAP_PROTO_DTLS);
            //if (!eps)
            //    ATLAS_LOGGER_ERROR("Cannot open COAP DTLS");
        }

        break;
    }

    if (!r) {
        //ATLAS_LOGGER_ERROR("Cannot start CoAP server");
        coap_free_context(ctx);
        return NULL;
    }

    freeaddrinfo(res);

    return ctx;
}

AtlasCoapServer::AtlasCoapServer(const std::string &hostname, const std::string &port,
                                 AtlasCoapServerMode mode, const std::string &psk)
{
    int fd;

    if (hostname == "")
        throw AtlasCoapException("Invalid hostname");
    if (port == "")
        throw AtlasCoapException("Invalid port");

    /* Start CoAP */
    coap_startup();

    /* Get CoAP context */
    ctx_ = this->getContext(hostname, port, mode, psk);

    /* Init default resources */
    this->initDefaultResources(ctx_);

    fd = coap_context_get_coap_fd(ctx_);
    if (fd == -1) {
        //ATLAS_LOGGER_INFO("Cannot get CoAP file descriptor");
        coap_free_context(ctx_);
        ctx_ = NULL;
        throw AtlasCoapException("Cannot get file descriptor for CoAP server");
    }

    AtlasScheduler::getInstance().addFdEntry(fd, std::bind(&AtlasCoapServer::schedulerCallback, this));

}

} // namespace atlas
