#include "AtlasFirewallPolicy.h"

namespace atlas {

namespace {

/* JSON policy key */
const std::string ATLAS_POLICY_JSON_KEY = "policy";

/* JSON policy-cliendId key */
const std::string ATLAS_POLICY_CLIENTID_JSON_KEY = "clientId";

/* JSON policy-qos key */
const std::string ATLAS_POLICY_QOS_JSON_KEY = "qos";

/* JSON policy-ppm key */
const std::string ATLAS_POLICY_PPM_JSON_KEY = "ppm";

/* JSON policy-payloadLen key */
const std::string ATLAS_POLICY_PAYLOADLEN_JSON_KEY = "payloadLen";

} // anonymous namespace

AtlasFirewallPolicy& AtlasFirewallPolicy::operator=(const AtlasFirewallPolicy& c)
{
    clientId_ = c.clientId_;
    qos_ = c.qos_;
    ppm_ = c.ppm_;
    payloadLen_ = c.payloadLen_;
    return *this;
}

bool AtlasFirewallPolicy::operator==(const AtlasFirewallPolicy& c)
{
    if((clientId_ == c.clientId_) && (qos_ == c.qos_) && (ppm_ = c.ppm_) && (payloadLen_ == c.payloadLen_))
        return true;
    else
        return false;
}

std::string AtlasFirewallPolicy::toJSON()
{
    return "\"" + ATLAS_POLICY_JSON_KEY + "\": \n" 
                + "{"
                + "\n\"" + ATLAS_POLICY_CLIENTID_JSON_KEY + "\": \"" + clientId_ + "\","
                + "\n\"" + ATLAS_POLICY_QOS_JSON_KEY + "\": \"" + std::to_string(qos_) + "\","
                + "\n\"" + ATLAS_POLICY_PPM_JSON_KEY + "\": \"" + std::to_string(ppm_) + "\","
                + "\n\"" + ATLAS_POLICY_PAYLOADLEN_JSON_KEY + "\": \"" + std::to_string(payloadLen_) + "\""
                + "\n}";
}

} // namespace atlas
