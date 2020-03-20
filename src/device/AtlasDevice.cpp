#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include "AtlasDevice.h"
#include "../logger/AtlasLogger.h"
#include "../telemetry/AtlasTelemetryInfo.h"
#include "../telemetry/AtlasAlertFactory.h"
#include "../pubsub_agent/AtlasPubSubAgent.h"
#include "../reputation_impl/AtlasDeviceFeatureType.h"

namespace atlas {

namespace {

/* Keep-alive counter initial value */
const int ATLAS_KEEP_ALIVE_COUNTER = 4;
/* JSON register event key */
const std::string ATLAS_REGISTER_JSON_KEY = "registered";
/* JSON last register time key */
const std::string ATLAS_LAST_REGISTER_TIME_JSON_KEY = "lastRegisterTime";
/* JSON last keep-alive time key */
const std::string ATLAS_LAST_KEEPALIVE_TIME_JSON_KEY = "lastKeepAliveTime";
/* JSON IP and port key */
const std::string ATLAS_IP_PORT_JSON_KEY = "ipPort";
/* JSON firewall stat key */
const std::string ATLAS_FIREWALLSTAT_JSON_KEY = "firewallStatistic";
/* JSON firewall stat-droppedPkts key */
const std::string ATLAS_FIREWALLSTAT_DROPPEDPKTS_JSON_KEY = "droppedPkts";
/* JSON firewall stat-passedPkts key */
const std::string ATLAS_FIREWALLSTAT_PASSEDPKTS_JSON_KEY = "passedPkts";
/* JSON system reputation key */
const std::string ATLAS_SYSTEM_REPUTATION_JSON_KEY = "systemReputation";
/* JSON data reputation key */
const std::string ATLAS_DATA_REPUTATION_JSON_KEY = "dataReputation";

} // anonymous namespace

AtlasDevice::AtlasDevice(const std::string &identity,
                         std::shared_ptr<AtlasDeviceCloud> deviceCloud) : identity_(identity),
                                                                          deviceCloud_(deviceCloud),
                                                                          registered_(false),
                                                                          regIntervalSec_(0),
                                                                          keepAlivePkts_(0)
{
    /* Install default alerts */
    installDefaultAlerts();

    AtlasDeviceFeatureManager sysManager;
    AtlasDeviceFeatureManager dataManager;
    deviceReputation_[AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL] = sysManager;
    deviceReputation_[AtlasDeviceNetworkType::ATLAS_NETWORK_DATA] = dataManager;
}

AtlasDevice::AtlasDevice() : identity_(""), registered_(false), regIntervalSec_(0), keepAlivePkts_(0) 
{
    AtlasDeviceFeatureManager sysManager;
    AtlasDeviceFeatureManager dataManager;
    deviceReputation_[AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL] = sysManager;
    deviceReputation_[AtlasDeviceNetworkType::ATLAS_NETWORK_DATA] = dataManager;
}

void AtlasDevice::uninstallPolicy()
{
    if (policy_) {
        /* Remove firewall rule from publish-subscribe broker */
        AtlasPubSubAgent::getInstance().removeFirewallRule(policy_->getClientId());
        /* Explicit delete policy */
        policy_.reset();
    }

    /* Explicit delete firewall statistics */
    stats_.reset();
}

void AtlasDevice::installDefaultAlerts()
{
    AtlasPushAlert *pushAlert;
    AtlasThresholdAlert *thresholdAlert;

    ATLAS_LOGGER_INFO1("Install default telemetry alerts for client with identity ", identity_);
    
    /* Install default push alerts */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_PROCS, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_PROCS] = std::unique_ptr<AtlasAlert>(pushAlert);
    
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_UPTIME, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_UPTIME] = std::unique_ptr<AtlasAlert>(pushAlert);

    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE, identity_);
    pushAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE] = std::unique_ptr<AtlasAlert>(pushAlert);

    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_PACKETS_INFO_PACKETS_AVG, identity_);
    pushAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_AVG] = std::unique_ptr<AtlasAlert>(pushAlert);

    /* Install default threshold alerts */
    thresholdAlert = AtlasAlertFactory::getThresholdAlert(TELEMETRY_SYSINFO_PROCS, identity_);
    thresholdAlerts_[TELEMETRY_SYSINFO_PROCS] = std::unique_ptr<AtlasAlert>(thresholdAlert);

    thresholdAlert = AtlasAlertFactory::getThresholdAlert(TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE, identity_);
    thresholdAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE] = std::unique_ptr<AtlasAlert>(thresholdAlert);

    thresholdAlert = AtlasAlertFactory::getThresholdAlert(TELEMETRY_PACKETS_INFO_PACKETS_AVG, identity_);
    thresholdAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_AVG] = std::unique_ptr<AtlasAlert>(thresholdAlert);
}

void AtlasDevice::pushAlerts()
{
    ATLAS_LOGGER_INFO1("Push all telemetry alerts to client with identity ", identity_);

    auto it = pushAlerts_.begin();
    while (it != pushAlerts_.end()) {
        ATLAS_LOGGER_INFO1("Push to client device telemetry push alert of type ", (*it).first);
        (*it).second->push();
        ++it;
    }
    
    it = thresholdAlerts_.begin();
    while (it != thresholdAlerts_.end()) {
        ATLAS_LOGGER_INFO1("Push to client device telemetry threshold alert of type ", (*it).first);
        (*it).second->push();
        ++it;
    }
}

void AtlasDevice::setIpPort(const std::string &ipPort)
{
    if (ipPort_ != ipPort) {
        ipPort_ = ipPort;
        /* Update the client IP and port to cloud */
        deviceCloud_->updateDevice(identity_, ipPortToJSON());
    }
}

void AtlasDevice::setPolicyInfo(std::unique_ptr<AtlasFirewallPolicy> policy)
{
    if(!policy) {
        ATLAS_LOGGER_ERROR("Received an empty policy!");
        return;
    }

    if((!policy_) || (!(*policy_ == *policy))) {
        policy_ = std::move(policy);
        deviceCloud_->updateDevice(identity_, policy_->toJSON());
    }
}

void AtlasDevice::setFirewallStats(std::unique_ptr<AtlasFirewallStats> stats)
{
    if(!stats) {
        ATLAS_LOGGER_ERROR("Received an empty firewall statistic!");
        return;
    }

    if((!stats_) || (!(*stats_ == *stats))) {
        stats_ = std::move(stats);
        deviceCloud_->updateDevice(identity_, stats_->toJSON());
    }    
}

int AtlasDevice::getRegInterval()
{
    /* If device is registered, then compute the registration time until now */
    if (registered_) {
        /* Get the registration time interval */
        boost::posix_time::time_duration diff = boost::posix_time::second_clock::local_time() - startRegTime_;
        return regIntervalSec_ +  diff.total_seconds();
    }

    return regIntervalSec_;
}

void AtlasDevice::registerNow()
{
    startRegTime_ = boost::posix_time::second_clock::local_time();
    regTime_ = boost::posix_time::to_simple_string(startRegTime_);

    registered_ = true;
    kaCtr_ = ATLAS_KEEP_ALIVE_COUNTER;

    /* Reset all telemetry features to default, the client should provide an update */
    telemetryInfo_.clearFeatures();

    ATLAS_LOGGER_INFO1("Client device registered at ", regTime_);

    /* Update registration event to cloud */
    deviceCloud_->updateDevice(identity_, toJSON());
}

void AtlasDevice::keepAliveNow()
{
    boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
    keepAliveTime_ = boost::posix_time::to_simple_string(timeLocal);

    kaCtr_ = ATLAS_KEEP_ALIVE_COUNTER;
    ATLAS_LOGGER_INFO1("Client device sent a keep-alive at ", keepAliveTime_);

    keepAlivePkts_++;

    /* Update keep-alive event to cloud */
    deviceCloud_->updateDevice(identity_, keepaliveEventToJSON());
}

void AtlasDevice::keepAliveExpired()
{
    if (!kaCtr_)
        return;

    kaCtr_--;
    if (!kaCtr_ && registered_) {
        ATLAS_LOGGER_INFO1("Keep-alive counter expired for client with identity ", identity_);
        /* Get the registration time interval */
        boost::posix_time::time_duration diff = boost::posix_time::second_clock::local_time() - startRegTime_;
        regIntervalSec_ += diff.total_seconds();

        registered_ = false;
        
        uninstallPolicy();
        /* Update un-registration event to cloud */
        deviceCloud_->updateDevice(identity_, registerEventToJSON());
    }
}

void AtlasDevice::syncReputation(AtlasDeviceNetworkType netType)
{
    /* Update the system reputation value to cloud */
    switch (netType)
    {
    case AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL :
        deviceCloud_->updateDevice(identity_, reputationToJSON(netType));
        break;
    case AtlasDeviceNetworkType::ATLAS_NETWORK_DATA:
        deviceCloud_->updateDevice(identity_, reputationToJSON(netType));
        break;
    }    
}

std::string AtlasDevice::reputationToJSON(AtlasDeviceNetworkType netType)
{   
    std::string tmpRet = "\"";
    switch (netType)
    {
    case AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL:
        tmpRet = tmpRet + ATLAS_SYSTEM_REPUTATION_JSON_KEY + "\": \"" + std::to_string(deviceReputation_[netType].getReputation()) + "\"";
        break;    
    case AtlasDeviceNetworkType::ATLAS_NETWORK_DATA:
        tmpRet = tmpRet + ATLAS_DATA_REPUTATION_JSON_KEY + "\": \"" + std::to_string(deviceReputation_[netType].getReputation()) + "\"";
        break;
    }
    return tmpRet; 
}

std::string AtlasDevice::registerEventToJSON()
{
    std::string regEvent;
    
    if (registered_)
        regEvent = "\"" + ATLAS_REGISTER_JSON_KEY + "\": " + "\"true\",";
    else
        regEvent = "\"" + ATLAS_REGISTER_JSON_KEY + "\": " + "\"false\",";

    regEvent += "\n\"" + ATLAS_LAST_REGISTER_TIME_JSON_KEY + "\": \"" + regTime_ + "\"";
    
    return regEvent;
}

std::string AtlasDevice::keepaliveEventToJSON()
{
    return "\"" + ATLAS_LAST_KEEPALIVE_TIME_JSON_KEY + "\": \"" + keepAliveTime_ + "\"";
}

std::string AtlasDevice::ipPortToJSON()
{
    return "\"" + ATLAS_IP_PORT_JSON_KEY + "\": \"" + ipPort_ + "\"";
}

std::string AtlasDevice::toJSON()
{
    std::string jsonDevice;

    /* Add registration state */
    jsonDevice += registerEventToJSON();
    /* Add keep-alive state */
    jsonDevice += ",\n" + keepaliveEventToJSON();
    /* Add IP and port */
    jsonDevice += ",\n" + ipPortToJSON();
    /* Add telemetry info */
    jsonDevice += ",\n" + telemetryInfo_.toJSON();
    /* Add system reputation */
    jsonDevice += ",\n" + reputationToJSON(AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL);
    /* Add data reputation */
    jsonDevice += ",\n" + reputationToJSON(AtlasDeviceNetworkType::ATLAS_NETWORK_DATA);
    /* Add firewall policy info */
    if (policy_)
        jsonDevice += ",\n" + policy_->toJSON();
    /* Add firewall statistics info */
    if (stats_)
        jsonDevice += ",\n" + stats_->toJSON();

    return jsonDevice;
}

std::string AtlasDevice::telemetryInfoToJSON()
{
    return telemetryInfo_.toJSON();
}

void AtlasDevice::setFeature(const std::string &featureType, const std::string &featureValue)
{
    if (featureType == "") {
        ATLAS_LOGGER_ERROR("Invalid feature type");
        return;
    }

    if (telemetryInfo_.getFeature(featureType) != featureValue) {
        ATLAS_LOGGER_INFO("Update cloud with information for feature " + featureType);
        telemetryInfo_.setFeature(featureType, featureValue);
        deviceCloud_->updateDevice(identity_, telemetryInfo_.toJSON(featureType));
    }
}

} // namespace atlas
