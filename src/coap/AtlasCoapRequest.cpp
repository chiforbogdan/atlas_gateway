#include "AtlasCoapRequest.h"

namespace atlas {

AtlasCoapRequest::AtlasCoapRequest(coap_context_t *context, coap_session_t *session,
                                   coap_request_callback_t callback)
{
    context_ = context;
    session_ = session;
    callback_ = callback;
}

AtlasCoapRequest::AtlasCoapRequest() : context_(nullptr), session_(nullptr), callback_(nullptr) {}

} // namespace atlas
