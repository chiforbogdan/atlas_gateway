#include "AtlasPacketPolicer.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

AtlasPacketPolicer::AtlasPacketPolicer(uint8_t maxQos, uint16_t pps, uint16_t maxPayloadLen) : statPktDrop_(0), statPktPass_(0)
{
    maxQos_ = maxQos;
    pps_ = pps;
    maxPayloadLen_ = maxPayloadLen;
}

bool AtlasPacketPolicer::filter(const AtlasPacket &pkt)
{
    if (pkt.getQos() > maxQos_)
        goto DROP;

    if (pkt.getPayloadLen() > maxPayloadLen_)
        goto DROP;

    /* TODO implement PPS */

    /* Accept packet */
    ATLAS_LOGGER_DEBUG("Packet for destination client id %s is allowed", pkt.getDstClientId());
    statPktPass_++;
    return true;

DROP:
    ATLAS_LOGGER_DEBUG("Packet for destination client id %s is dropped", pkt.getDstClientId());
    /* Drop packet */
    statPktDrop_++;
    return false;
}

} // namespace atlas