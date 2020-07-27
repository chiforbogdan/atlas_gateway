#include <random>
#include <boost/bind.hpp>
#include <json/json.h>
#include "AtlasClaim.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

namespace {

const std::string ATLAS_CLAIM_REQUEST_PATH = "/gateway/claim";
const std::string ATLAS_CLAIM_SHORT_CODE_JSON_KEY = "short_code";
const std::string ATLAS_CLAIM_SECRET_KEY_JSON_KEY = "secret_key";
const std::string ATLAS_CLAIM_OWNER_IDENTIFIER_JSON_KEY = "owner_identifier";
const std::string ATLAS_CLAIM_SHORT_CODE_ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
int ATLAS_CLAIM_SHORT_CODE_LEN = 6;
int ATLAS_CLAIM_ALARM_PERIOD_MS = 180000; // change short code every 3 minutes

} // anonymous namespace

AtlasClaim::AtlasClaim() : shortCodeAlarm_("AtlasClaim", ATLAS_CLAIM_ALARM_PERIOD_MS, false,
                                           boost::bind(&AtlasClaim::alarmCallback, this)),
                           claimCallback_(AtlasHttpMethod::ATLAS_HTTP_POST, ATLAS_CLAIM_REQUEST_PATH,
                                          boost::bind(&AtlasClaim::handleClaimReq, this, _1, _2, _3))
{

    /* Generate short code */
    shortCode_ = generateShortCode();
    ATLAS_LOGGER_INFO("Claim protocol short code is: " + shortCode_);

    /* Start short code alarm */
    shortCodeAlarm_.start();

    /* Register callback to HTTP server */
    AtlasHttpServer::getInstance().addCallback(claimCallback_);

}

std::string AtlasClaim::generateShortCode()
{
    std::string str(ATLAS_CLAIM_SHORT_CODE_ALPHABET);
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, ATLAS_CLAIM_SHORT_CODE_LEN);
}

void AtlasClaim::alarmCallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for claiming protocol");

    /* Generate short code */
    shortCode_ = generateShortCode();
    ATLAS_LOGGER_INFO("Claim protocol short code is: " + shortCode_);
}

AtlasHttpResponse AtlasClaim::handleClaimReq(AtlasHttpMethod method, const std::string &path,
                                             const std::string &payload)
{
    Json::Reader reader;
    Json::Value obj;

    ATLAS_LOGGER_DEBUG("Handle claim request on path: " + path);

    if (payload == "") {
        ATLAS_LOGGER_ERROR("Rejecting claim request because payload is empty");
        return AtlasHttpResponse(401);
    }

    reader.parse(payload, obj);

    if (obj[ATLAS_CLAIM_SHORT_CODE_JSON_KEY].asString() != shortCode_) {
        ATLAS_LOGGER_ERROR("Rejecting claim request because the short code is invalid!");
        return AtlasHttpResponse(401, "Invalid short code");
    } 

    return AtlasHttpResponse(200, "claimed");
}

} // namespace atlas
