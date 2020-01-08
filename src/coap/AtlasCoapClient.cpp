#include "AtlasCoapClient.h"
#include "AtlasCoapException.h"
#include "../logger/AtlasLogger.h"

#define ATLAS_COAP_CLIENT_TMP_BUF_LEN (128)

namespace atlas {

uint32_t AtlasCoapClient::token_ = 0;

uint32_t AtlasCoapClient::getToken()
{
    return token_++;
}

int AtlasCoapClient::resolveAddress(coap_str_const_t *hostname, struct sockaddr *dst)
{
    struct addrinfo *res, *ainfo;
    struct addrinfo hints;
    char *addr;
    int status;
    int len;
 
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_UNSPEC;
 
    addr = new char[hostname->length + 1];
    memset(addr, 0, hostname->length + 1);
    memcpy(addr, hostname->s, hostname->length);
 
    status = getaddrinfo(addr, NULL, &hints, &res);
 
    delete[] addr;
 
    if (status) {
        ATLAS_LOGGER_ERROR("Cannot resolve CoAP request address");
        return -1;
    }
 
    for (ainfo = res; ainfo; ainfo = ainfo->ai_next) {
        if (ainfo->ai_family == AF_INET || ainfo->ai_family == AF_INET6) {
            memcpy(dst, ainfo->ai_addr, ainfo->ai_addrlen);
            len = ainfo->ai_addrlen;
            break;
        }
    }
 
    freeaddrinfo(res);
 
    return len;
}


coap_session_t* AtlasCoapClient::getSession(coap_context_t *ctx, coap_proto_t proto, coap_address_t *dst)
{
    if (proto == COAP_PROTO_DTLS)
        return coap_new_client_session_psk2(ctx, NULL, dst, proto, &dtlsPsk_);

    return coap_new_client_session(ctx, NULL, dst, proto);
}


void AtlasCoapClient::sendRequest(const std::string &uri, AtlasCoapMethod method, const uint8_t *reqPayload,
                 size_t reqPayloadLen, uint32_t timeout, coap_request_callback_t callback)
{

    coap_uri_t coapUri;
    coap_str_const_t hostname;
    coap_address_t dst;
    coap_context_t *ctx = NULL;
    coap_session_t *session = NULL;
    coap_pdu_t *reqPdu = NULL;
    coap_optlist_t *options = NULL;
    int res;
    uint32_t token;
    uint8_t buf[ATLAS_COAP_CLIENT_TMP_BUF_LEN];
    uint8_t *bufTmp;
    size_t buflen;

    ATLAS_LOGGER_DEBUG("Send CoAP request");
    
    try {

        if (coap_split_uri((uint8_t *)uri.c_str(), uri.length(), &coapUri) < 0)
            throw "Error encountered when parsing CoAP URI";

        /* Resolve CoAP hostname */
        hostname = coapUri.host;
        res = resolveAddress(&hostname, &dst.addr.sa);
        if (res < 0)
            throw "Cannot resolve CoAP address";

        /* Context init */
        ctx = coap_new_context(NULL);
        if (!ctx)
            throw "Cannot create CoAP context for client request";

        /* Register handlers */

        dst.size = res;
        dst.addr.sin.sin_port = htons(coapUri.port);

        /* Get session */
        if (coapUri.scheme == COAP_URI_SCHEME_COAPS)
            session = getSession(ctx, COAP_PROTO_DTLS, &dst);
        else
            session = getSession(ctx, COAP_PROTO_UDP, &dst);

        if (!session)
            throw "Cannot create CoAP session for client request";

        /* Create request PDU */
        if (!(reqPdu = coap_new_pdu(session)))
            throw "Cannot create client request PDU";
 
        reqPdu->type = COAP_MESSAGE_CON;
        reqPdu->tid = coap_new_message_id(session);

        switch(method) {
            case ATLAS_COAP_METHOD_GET:
                reqPdu->code = COAP_REQUEST_GET;
                break;

            case ATLAS_COAP_METHOD_POST:
                reqPdu->code = COAP_REQUEST_POST;
                break;

            case ATLAS_COAP_METHOD_PUT:
                reqPdu->code = COAP_REQUEST_PUT;
                break;

            case ATLAS_COAP_METHOD_DELETE:
                reqPdu->code = COAP_REQUEST_DELETE;
                break;

            default:
                reqPdu->code = COAP_REQUEST_GET;
        }

        /* Add token */
        token = getToken();
        if (!coap_add_token(reqPdu, sizeof(uint32_t), (uint8_t*) &token))
            throw "Cannot add token to CoAP client request";

        /* Add URI path */
        if (coapUri.path.length) {
            buflen = sizeof(buf);
            bufTmp = buf;
            res = coap_split_path(coapUri.path.s, coapUri.path.length, bufTmp, &buflen);

            while (res--) {
                coap_insert_optlist(&options,
                                    coap_new_optlist(COAP_OPTION_URI_PATH,
                                    coap_opt_length(bufTmp),
                                    coap_opt_value(bufTmp)));

                bufTmp += coap_opt_size(bufTmp);
            }
        }

        /* Add URI query */
        if (coapUri.query.length) {
            buflen = sizeof(buf);
            bufTmp = buf;
            res = coap_split_query(coapUri.query.s, coapUri.query.length, bufTmp, &buflen);

            while (res--) {
                coap_insert_optlist(&options,
                                    coap_new_optlist(COAP_OPTION_URI_QUERY,
                                    coap_opt_length(buf),
                                    coap_opt_value(buf)));

                bufTmp += coap_opt_size(bufTmp);
            }
        }

        if (options) {
            coap_add_optlist_pdu(reqPdu, &options);
            coap_delete_optlist(options);
        }

        /* Add request payload */
        if (reqPayload && reqPayloadLen)
            coap_add_data(reqPdu, reqPayloadLen, reqPayload);

        /* Send request */
        ATLAS_LOGGER_DEBUG("Sending CoAP client request...");
        coap_send(session, reqPdu);

    } catch(const char *e) {
        ATLAS_LOGGER_ERROR(e);

        coap_session_release(session);
        coap_free_context(ctx);

        throw AtlasCoapException(e);
    }
}

void
AtlasCoapClient::setDtlsInfo(const std::string &identity, const std::string &psk)
{
    memset(&dtlsPsk_, 0, sizeof(dtlsPsk_));

    dtlsPsk_.version = COAP_DTLS_CPSK_SETUP_VERSION;
     
    /* Set DTLS identity */
    dtlsIdentity_ = identity;
    dtlsPsk_.psk_info.identity.s = (uint8_t *) dtlsIdentity_.c_str();
    dtlsPsk_.psk_info.identity.length = dtlsIdentity_.length();

    /* Set DTLS key */
    dtlsKey_ = psk;
    dtlsPsk_.psk_info.key.s = (uint8_t *) dtlsKey_.c_str();
    dtlsPsk_.psk_info.key.length = dtlsKey_.length();
}

} // namespace atlas

