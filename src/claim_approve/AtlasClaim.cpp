#include <boost/bind.hpp>
#include "AtlasClaim.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

namespace {

const std::string ATLAS_CLAIM_REQUEST_PATH = "/gateway/claim";

} // anonymous namespace

AtlasClaim::AtlasClaim() : shortCodeAlarm_("AtlasClaim", 5000, false,
                                           boost::bind(&AtlasClaim::alarmCallback, this)),
                           claimCallback_(AtlasHttpMethod::ATLAS_HTTP_POST, ATLAS_CLAIM_REQUEST_PATH,
                                          boost::bind(&AtlasClaim::handleClaimReq, this, _1, _2, _3))
{
    /* Start short code alarm */
    shortCodeAlarm_.start();

    /* Register callback to HTTP server */
    AtlasHttpServer::getInstance().addCallback(claimCallback_);

}

void AtlasClaim::alarmCallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for claiming protocol");
}

AtlasHttpResponse AtlasClaim::handleClaimReq(AtlasHttpMethod method, const std::string &path,
                                             const std::string &payload)
{
    ATLAS_LOGGER_DEBUG("Handle claim request on path: " + path);

    return AtlasHttpResponse(200, "claimed");
}


} // namespace atlas
