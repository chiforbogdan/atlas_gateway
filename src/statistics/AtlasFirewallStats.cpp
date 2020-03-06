#include "AtlasFirewallStats.h"


namespace atlas {

namespace {

/* JSON firewall stat key */
const std::string ATLAS_FIREWALLSTATS_JSON_KEY = "firewallStatistics";

/* JSON firewall stat-droppedPkts key */
const std::string ATLAS_FIREWALLSTATS_DROPPEDPKTS_JSON_KEY = "droppedPkts";

/* JSON firewall stat-passedPkts key */
const std::string ATLAS_FIREWALLSTATS_PASSEDPKTS_JSON_KEY = "passedPkts";

} // anonymous namespace



AtlasFirewallStats& AtlasFirewallStats::operator=(const AtlasFirewallStats& c)
{
    clientId_ = c.clientId_;
    droppedPkts_ = c.droppedPkts_;
    passedPkts_ = c.passedPkts_;
    return *this;
}

bool AtlasFirewallStats::operator==(const AtlasFirewallStats& c)
{
    if((clientId_ == c.clientId_) && (droppedPkts_ == c.droppedPkts_) && (passedPkts_ == c.passedPkts_))
        return true;
    else
        return false;
}

std::string AtlasFirewallStats::toJSON()
{
    return "\"" + ATLAS_FIREWALLSTATS_JSON_KEY + "\": \n" 
                + "{"
                + "\n\"" + ATLAS_FIREWALLSTATS_DROPPEDPKTS_JSON_KEY + "\": \"" + std::to_string(droppedPkts_) + "\","
                + "\n\"" + ATLAS_FIREWALLSTATS_PASSEDPKTS_JSON_KEY + "\": \"" + std::to_string(passedPkts_) + "\""
                + "\n}";
}

} // namespace atlas
