#ifndef __ATLAS_RECEIVE_FEEDBACK_H__
#define __ATLAS_RECEIVE_FEEDBACK_H__

#include "../coap/AtlasCoapResponse.h"
#include "../coap/AtlasCoapMethod.h"
#include "../coap/AtlasCoapResource.h"

namespace atlas {

class AtlasReceiveFeedback
{

public:
    /**
    * @brief Ctor for feedback
    * @return none
    */
    AtlasReceiveFeedback();

    /**
    * @brief Register feedback resource
    * @return none
    */
    void start();

    /**
    * @brief Un-register feedback resource
    * @return none
    */
    void stop();

private:

    /**
    * @brief Feedback callback
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
    AtlasCoapResponse receiveFeedbackCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                                AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                                uint8_t **respPayload, size_t *respPayloadLen);

    /* Feedback command CoAP resource*/
    AtlasCoapResource receiveFeedbackResource_;
};

} // namespace atlas

#endif /* __ATLAS_RECEIVE_FEEDBACK_H__ */
