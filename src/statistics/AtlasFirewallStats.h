#ifndef __ATLAS_FIREWALL_STATS_H__
#define __ATLAS_FIREWALL_STATS_H__

#include <string>
#include "../pubsub_agent/AtlasPubSubAgent.h"

namespace atlas {

class AtlasFirewallStats
{

public:
    /**
    * @brief Default ctor for firewall statistics
    * @return none
    */
    AtlasFirewallStats() : clientId_(""){}

    /**
    * @brief Ctor for firewall statistics
    * @param[in] clientId 
    * @param[in] dropped Packets
    * @param[in] passed Packets
    * @return none
    */
    AtlasFirewallStats(const std::string &clientId, uint32_t droppedPkts, uint32_t passedPkts) : clientId_(clientId), droppedPkts_(droppedPkts), passedPkts_(passedPkts){}

    /**
    * @brief Overwrite = operator
    * @param[in] AtlasFirewallStats reference
    * @return referece to this
    */
    AtlasFirewallStats& operator=(const AtlasFirewallStats& c);

    /**
    * @brief Overwrite == operator
    * @param[in] AtlasFirewallStats reference
    * @return true or false
    */
    bool operator==(const AtlasFirewallStats& c);

    /**
    * @brief Get firewall stats clientId
    * @return clientId
    */
    inline std::string getClientId() const { return clientId_; }

    /**
    * @brief Get firewall stats droppedPkts
    * @return droppedPkts
    */
    inline uint32_t getDroppedPkts() const { return droppedPkts_; }

    /**
    * @brief Get firewall stats passedPkts
    * @return passedPkts
    */
    inline uint32_t getPassedPkts() const { return passedPkts_; }

    /**
    * @brief Set firewall statistics
    * @param[in] clientId
    * @param[in] dropped packets
    * @param[in] passed packets
    * @return none
    */
    void setFirewallStats(const std::string &clientId, uint32_t droppedPkts, uint32_t passedPkts);

    /**
    * @brief Put all stats info in json format
    * @return string
    */
    std::string toJSON();

private:
    /* Policy clientid*/
    std::string clientId_;

    /* Firewall statistic param - droppedPkts*/
    uint32_t droppedPkts_;

    /* Firewall statistic param - passedPkts*/
    uint32_t passedPkts_;
};

} // namespace atlas

#endif /* __ATLAS_FIREWALL_STATS_H__ */
