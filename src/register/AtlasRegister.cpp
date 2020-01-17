#include <boost/bind.hpp>
#include "AtlasRegister.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

const std::string ATLAS_REGISTER_URI = "gateway/register";
const std::string ATLAS_KEEPALIVE_URI = "gateway/keepalive";

AtlasRegister::AtlasRegister() : registerResource_(ATLAS_REGISTER_URI, ATLAS_COAP_METHOD_POST, boost::bind(&AtlasRegister::registerCallback, this, _1, _2, _3, _4, _5, _6)),
                                 keepAliveResource_(ATLAS_KEEPALIVE_URI, ATLAS_COAP_METHOD_PUT, boost::bind(&AtlasRegister::keepaliveCallback, this, _1, _2, _3, _4, _5, _6)) {}

AtlasCoapResponse AtlasRegister::keepaliveCallback(std::string path, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    ATLAS_LOGGER_DEBUG("Keepalive callback executed...");

    return ATLAS_COAP_RESP_OK;
}


AtlasCoapResponse AtlasRegister::registerCallback(std::string path, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    ATLAS_LOGGER_DEBUG("Register callback executed...");

    return ATLAS_COAP_RESP_OK;
}

void AtlasRegister::start()
{

    ATLAS_LOGGER_DEBUG("Start registration module");

    /* Add REGISTER and KEEPALIVE resource to CoAP server. This is called by the IoT device in the 
       registration phase and for keepalive pings after that.*/
    AtlasCoapServer::getInstance().addResource(registerResource_);
    AtlasCoapServer::getInstance().addResource(keepAliveResource_);
}

void AtlasRegister::stop()
{
    ATLAS_LOGGER_DEBUG("Stop registration module");
    
    AtlasCoapServer::getInstance().delResource(registerResource_);
    AtlasCoapServer::getInstance().delResource(keepAliveResource_);
}

} // namespace atlas
