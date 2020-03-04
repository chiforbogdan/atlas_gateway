#ifndef __ATLAS_FIREWALL_POLICY_H__
#define __ATLAS_FIREWALL_POLICY_H__

#include <string>
#include "../pubsub_agent/AtlasPubSubAgent.h"

namespace atlas {

class AtlasFirewallPolicy
{

public:
    /**
    * @brief Default ctor for firewall policy
    * @return none
    */
    AtlasFirewallPolicy() : clientId_(""){}

    /**
    * @brief Ctor for firewall policy
    * @param[in] clientId - rule id
    * @param[in] qos - rule qos
    * @param[in] ppm - rule ppm
    * @param[in] payloadLen - rule payloadLen
    * @return none
    */
    AtlasFirewallPolicy(const std::string &clientId, uint16_t qos, uint16_t ppm, uint16_t payloadLen) : clientId_(clientId), qos_(qos),
                                                                                                         ppm_(ppm), payloadLen_(payloadLen){}

    /**
    * @brief Overwrite = operator
    * @param[in] AtlasFirewallPolicy reference
    * @return referece to this
    */
    AtlasFirewallPolicy& operator=(const AtlasFirewallPolicy& c);

    /**
    * @brief Overwrite == operator
    * @param[in] AtlasFirewallPolicy reference
    * @return true or false
    */
    bool operator==(const AtlasFirewallPolicy& c);

    /**
    * @brief Set policy info for client device
    * @param[in] client id
    * @param[in] qos
    * @param[in] ppm
    * @param[in] payloadLen
    * @return none
    */
    void setPolicyInfo(const std::string &clientId, uint16_t qos, uint16_t ppm, uint16_t payloadLen);

    /**
    * @brief Get policy clientId
    * @return clientId
    */
    inline std::string getClientId() const { return clientId_; }

    /**
    * @brief Get policy qos
    * @return qos
    */
    inline uint16_t getQOS() const { return qos_; }

    /**
    * @brief Get policy ppm
    * @return ppm
    */
    inline uint16_t getPPM() const { return ppm_; }

    /**
    * @brief Get policy payloadLen
    * @return payloadLen
    */
    inline uint16_t getPayloadLen() const { return payloadLen_; }
    
    /**
    * @brief Put all firewall policy info in json format
    * @return string
    */
    std::string toJSON();

private:
    /* Policy info - clientid*/
    std::string clientId_;

    /* Policy info - qos*/
    uint16_t qos_;

    /* Policy info - ppm*/
    uint16_t ppm_;

    /* Policy info - payloadLen*/
    uint16_t payloadLen_;
};

} // namespace atlas

#endif /* __ATLAS_FIREWALL_POLICY_H__ */
