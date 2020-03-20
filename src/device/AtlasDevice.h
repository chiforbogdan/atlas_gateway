#ifndef __ATLAS_DEVICE_H__
#define __ATLAS_DEVICE_H__

#include <string>
#include <memory>
#include <boost/optional.hpp>
#include <unordered_map>
#include "../telemetry/AtlasTelemetryInfo.h"
#include "../telemetry/AtlasAlert.h"
#include "../commands/AtlasCommandType.h"
#include "../cloud/AtlasDeviceCloud.h"
#include "../policy/AtlasFirewallPolicy.h"
#include "../statistics/AtlasFirewallStats.h"
#include "../reputation_impl/AtlasDeviceFeatureManager.h"

namespace atlas {

const std::string ATLAS_COAPS_SCHEME = "coaps://";

class AtlasDevice
{

public:
    /**
    * @brief Default ctor for client device
    * @return none
    */
    AtlasDevice();

    /**
    * @brief Ctor for client device
    * @param[in] identity Client device identity
    * @param[in] deviceCloud Cloud back-end manager
    * @return none
    */
    AtlasDevice(const std::string &identity, std::shared_ptr<AtlasDeviceCloud> deviceCloud);

    /**
    * @brief Set PSK for client device
    * @param[in] psk Pre-shared key
    * @return none
    */
    inline void setPsk(const std::string &psk) { psk_ = psk; }

    /**
    * @brief Set IP for client device
    * @param[in] ip address
    * @return none
    */
    void setIpPort(const std::string &ipPort);

    /**
    * @brief Set policy info for client device
    * @param[in] policy pointer
    * @return none
    */
    void setPolicyInfo(std::unique_ptr<AtlasFirewallPolicy> policy);

    /**
    * @brief Set firewall statistic for client device
    * @param[in] statistics pointer
    * @return none
    */
    void setFirewallStats(std::unique_ptr<AtlasFirewallStats> stats);

    /**
    * @brief Get client device identity
    * @return Client device identity
    */
    inline std::string getIdentity() const { return identity_; }

    /**
    * @brief Get client device pre-shared key 
    * @return Client device pre-shared key
    */
    inline std::string getPsk() const { return psk_; }

    /**
    * @brief Get client device pre-shared key as char array
    * @return Client device pre-shared key as char array
    */
    inline const char *getPskAsCharArray() const { return psk_.c_str(); }

    /**
    * @brief Get client device registration time
    * @return Client device registration time
    */
    inline std::string getRegTime() const { return regTime_; }

    /**
    * @brief Get client device policy
    * @return Client device policy
    */
    inline const AtlasFirewallPolicy* getPolicy() const { return policy_.get(); }

    /**
    * @brief Get client device firewall stats
    * @return Client device firewall stats
    */
    inline const AtlasFirewallStats* getFirewallStats() const { return stats_.get(); }

    /**
    * @brief Indicate that a client device just registered
    * @return none
    */
    void registerNow();

    /**
    * @brief Indicate that a client device just sent a keep-alive
    * @return none
    */
    void keepAliveNow();

    /**
    * @brief Keep-alive expired timer callback. This will decrement the internal keep-alive counter
    * @return none
    */
    void keepAliveExpired();

    /**
    * @brief Get the client device registration state
    * @return True if the device is registered, false otherwise
    */
    inline bool isRegistered() const { return registered_; }

    /**
    * @brief Push all telemetry alerts on client device
    * @return none
    */
    void pushAlerts();

    /**
    *@brief Get device URL
    * @return Client device URL
    */
    inline std::string getUrl() const { return ATLAS_COAPS_SCHEME + ipPort_; }

    /**
    * @brief Set feature value
    * @param[in] featureType Feature type
    * @param[in] featureValue Feature value
    * @return none
    */
    void setFeature(const std::string &featureType, const std::string &featureValue);

    /**
    * @brief Put all device info in json format
    * @return JSON serialized device info
    */
    std::string toJSON();

    /**
    * @brief Serialize reputation info to JSON
    * @param[in] netType Reputation network type
    * @return JSON serialized system reputation info
    */
    std::string reputationToJSON(AtlasDeviceNetworkType netType);

    /**
    * @brief Serialize telemetry info to JSON
    * @return JSON serialized telemetry info
    */
    std::string telemetryInfoToJSON();

    /**
    * @brief Indicates if a full cloud sync is required
    * @return True if a sync is required, false otherwise
    */
    inline bool isSyncRequired() const { return syncRequired_; }

    /**
    * @brief Enable full cloud sync
    * @return none
    */
    inline void setSyncRequired() { syncRequired_ = true; };

    /**
    * @brief Disable full cloud sync
    * @return none
    */
    inline void clearSyncRequired() { syncRequired_ = false; }

    /**
    * @brief Get registration time interval
    * @return Registration time interval (seconds)
    */
    int getRegInterval();

    /**
    * @brief Get the number of received keep-alive packets
    * @return Number of keep-alive packets
    */
    inline int getKeepalivePkts() const { return keepAlivePkts_; }

    /**
    * @brief Get the reputation network
    * @param[in] netType Reputation network type
    * @return A reference to the system reputation
    */
    AtlasDeviceFeatureManager& getReputation(AtlasDeviceNetworkType netType) { return deviceReputation_[netType]; }

    /**
    * @brief Sync reputation with the cloud back-end
    * @param[in] netType Reputation network type
    * @return none
    */
    void syncReputation(AtlasDeviceNetworkType netType);
private:
    /**
    * @brief Install default telemetry alerts
    * @return none
    */
    void uninstallPolicy();

    /**
    * @brief Install default telemetry alerts
    * @return none
    */
    void installDefaultAlerts();

    /**
    * @brief Serialize register event to JSON
    * @return JSON serialized register event
    */
    std::string registerEventToJSON();

    /**
    * @brief Serialize keepalive event to JSON
    * @return JSON serialized keepalive event
    */
    std::string keepaliveEventToJSON();

    /**
    * @brief Serialize client IP and port to JSON
    * @return JSON serialized IP and port
    */
    std::string ipPortToJSON();

    /* IoT client identity */
    std::string identity_;

    /* IoT client pre-shared key */
    std::string psk_;

    /* IoT client ip adrress & port */
    std::string ipPort_;

    /* Registration timestamp */
    std::string regTime_;

    /* Keep-alive timestamp */
    std::string keepAliveTime_;

    /* Cloud back-end manager */
    std::shared_ptr<AtlasDeviceCloud> deviceCloud_;

    /* Indicates if the device is registered */
    bool registered_;

    /* Keep-alive counter. When this counter reaches 0, the device is de-registered */
    uint8_t kaCtr_;

    /* Indicates if a cloud back-end full sync is required */
    bool syncRequired_;

    /* Telemetry info */
    AtlasTelemetryInfo telemetryInfo_;

    /* Telemetry push alerts */
    std::unordered_map<std::string, std::unique_ptr<AtlasAlert> > pushAlerts_;

    /* Telemetry threshold alerts */
    std::unordered_map<std::string, std::unique_ptr<AtlasAlert> > thresholdAlerts_;

    /* Policy*/
    std::unique_ptr<AtlasFirewallPolicy> policy_;

    /* Firewall statistic*/
    std::unique_ptr<AtlasFirewallStats> stats_;

    /* Holds the most recent registration timestamp */
    boost::posix_time::ptime startRegTime_;

    /* Holds the sum of registration time intervals */
    int regIntervalSec_;

    /* Counts the number of received keep-alive packets */
    int keepAlivePkts_;

    /* System reputation */
    std::unordered_map<AtlasDeviceNetworkType, AtlasDeviceFeatureManager> deviceReputation_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_H__ */
