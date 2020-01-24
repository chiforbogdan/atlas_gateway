#ifndef __ATLAS_PACKET_POLICER_H__
#define __ATLAS_PACKET_POLICER_H__

#include <stdint.h>
#include "AtlasPacketPolicer.h"
#include "AtlasPacket.h"

namespace atlas {

class AtlasPacketPolicer
{

public:

    /**
    * @brief Default ctor for packet policer
    * @return none
    */
    AtlasPacketPolicer() {}

    /**
    * @brief Ctor for packet policer
    * @param[in] maxQoS Max QoS value
    * @param[in] pps Allowed packets per second
    * @param[in] maxPayloadLen Max payload length
    * @return none
    */
    AtlasPacketPolicer(uint8_t maxQos, uint16_t pps, uint16_t maxPayloadLen);
    
    /**
    * @brief Filter publis-subscribe packet
    * @param[in] pkt Publish-subscribe packet
    * @return True if packet should be processed, False if packet should be dropped
    */
    bool filter(const AtlasPacket &pkt);

    /**
    * @brief Get number of dropped packets
    * @return Number of dropped packets
    */
    uint32_t getStatDroppedPkt() const { return statPktDrop_; }

    /**
    * @brief Get number of passed packets
    * @return Number of passed packets
    */
    uint32_t getStatPassedPkt() const { return statPktPass_; }

private:
    /* Qualifiers */
    
    /* Maximum QoS value */
    uint8_t maxQos_;
    
    /* Allowed number of packets per second */
    uint16_t pps_;

    /* Maximum payload length*/
    uint16_t maxPayloadLen_;

    /* Statistics */

    /* Number of dropped packets */
    uint32_t statPktDrop_;

    /* Number of passed packets */
    uint32_t statPktPass_;
};

} // namespace atlas

#endif /* __ATLAS_PACKET_POLICER_H__ */
