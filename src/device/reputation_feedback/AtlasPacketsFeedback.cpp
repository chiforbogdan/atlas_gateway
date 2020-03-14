#include "AtlasPacketsFeedback.h"
#include "../AtlasDeviceManager.h"
#include "../../logger/AtlasLogger.h"

#include <iostream>

namespace atlas {

namespace {

const int ATLAS_PACKETS_DEFAULT_FEEDBACK = 0.4;

} // anonymous namespace

double AtlasPacketsFeedback::getFeedback()
{
    uint32_t txDroppedPkts;
    uint32_t txPassedPkts;
    const AtlasFirewallStats* stats = device_.getFirewallStats();
    double feedback;

    /* If no statistics are available, then return the default feedback */ 
    if (!stats)
        return ATLAS_PACKETS_DEFAULT_FEEDBACK;

    txDroppedPkts = stats->getTxDroppedPkts();
    txPassedPkts = stats->getTxPassedPkts();

    /* Compute the difference within the sample window */
    if (txDroppedPkts >= prevStats_.getTxDroppedPkts())
        txDroppedPkts -= prevStats_.getTxDroppedPkts();
    if (txPassedPkts >= prevStats_.getTxPassedPkts())
        txPassedPkts -= prevStats_.getTxPassedPkts();
    
    /* If no packets are transmitted, then return the default feedback */
    if (!(txDroppedPkts + txPassedPkts))
        return ATLAS_PACKETS_DEFAULT_FEEDBACK;

    feedback = (double) txPassedPkts / (double) (txDroppedPkts + txPassedPkts);

    prevStats_ = *stats;
 
    ATLAS_LOGGER_INFO("Feedback was computed for valid TX packets (system reputation)");

    return feedback;
}

} // namespace atlas