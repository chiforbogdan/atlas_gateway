#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasDeviceManager.h"
#include "../logger/AtlasLogger.h"
#include "../scheduler/AtlasScheduler.h"
#include "../reputation_impl/AtlasReputation_NaiveBayes.h"
#include "../register/AtlasRegister.h"
#include "reputation_feedback/AtlasRegistrationFeedback.h"
#include "reputation_feedback/AtlasKeepaliveFeedback.h"
#include "reputation_feedback/AtlasPacketsFeedback.h"
#include "../sql/AtlasSQLite.h"
#include "../reputation_impl/AtlasDeviceFeatureManager.h"

namespace atlas {

namespace {

const int ATLAS_FIREWALL_STATISTICS_INTERVAL_MS = 60000;
const int ATLAS_SYSTEM_REPUTATION_INTERVAL_MS = 60000;

/* Reputation feature weights (importance) */
const double ATLAS_VALID_PACKETS_WEIGHT = 0.3;
const double ATLAS_REGISTER_TIME_WEIGHT = 0.4;
const double ATLAS_KEEPALIVE_PACKETS_WEIGHT = 0.3;

/* System reputation threshold value */
const double ATLAS_SYSTEM_REPUTATION_THRESHOLD = 0.8;

} // anonymous namespace

AtlasDeviceManager& AtlasDeviceManager::getInstance()
{
    static AtlasDeviceManager instance;

    return instance;
}

AtlasDeviceManager::AtlasDeviceManager(): deviceCloud_(new AtlasDeviceCloud()),
                                          fsAlarm_(ATLAS_FIREWALL_STATISTICS_INTERVAL_MS, false,
                                                   boost::bind(&AtlasDeviceManager::firewallStatisticsAlarmCallback, this)),
                                          sysRepAlarm_(ATLAS_SYSTEM_REPUTATION_INTERVAL_MS, false,
                                                       boost::bind(&AtlasDeviceManager::sysRepAlarmCallback, this))
{
    /* Start firewall statistics alarm */
    fsAlarm_.start();
    /* Start system reputation alarm */
    sysRepAlarm_.start();
}

void AtlasDeviceManager::firewallStatisticsAlarmCallback()
{
    ATLAS_LOGGER_INFO("Firewall-statistics alarm callback");

    forEachDevice([] (AtlasDevice& device) 
                     {     
                         if(device.getPolicy())
                             AtlasPubSubAgent::getInstance().getFirewallRuleStats((device.getPolicy()->getClientId()));
                     });
}

void AtlasDeviceManager::subAlarmCallback(AtlasDevice& device)
{
    std::vector<std::pair<AtlasDeviceFeatureType, double>> feedbackMatrix;
    /* Parse each system reputation feedback element and get feedback for device */
    for (auto &feedbackElem : feedback_[device.getIdentity()])
        feedbackMatrix.push_back(std::pair<AtlasDeviceFeatureType, double>(feedbackElem->getType(),
                                                                        feedbackElem->getFeedback()));

    /* Compute system reputation using naive-bayes */
    double repVal = AtlasReputationNaiveBayes::computeReputation(device.getSystemReputation(), feedbackMatrix);
    ATLAS_LOGGER_INFO("System reputation for device with identity " + device.getIdentity() + 
                    " is " + std::to_string(repVal));
    device.syncSystemReputation();

    if(!AtlasSQLite::getInstance().updateNetwork(device.getIdentity(), (int)AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL, device.getSystemReputation())){
         ATLAS_LOGGER_ERROR("Uncommited update on naiveBayesNetwork table");
    }
    if(!AtlasSQLite::getInstance().updateFeatures(device.getIdentity(), (int)AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL, device.getSystemReputation())){
         ATLAS_LOGGER_ERROR("Uncommited update on naiveBayesFeature table");
    }
}

void AtlasDeviceManager::sysRepAlarmCallback()
{
    ATLAS_LOGGER_INFO("System reputation alarm callback");

    forEachDevice([this] (AtlasDevice& device) 
                         {     
                            subAlarmCallback(device);
                         });
}

void AtlasDeviceManager::initSystemReputation(AtlasDevice &device)
{
    /* Add default features for the system reputation */
    AtlasDeviceFeatureManager &systemReputation = device.getSystemReputation();
    
    systemReputation.updateFeedbackThreshold(ATLAS_SYSTEM_REPUTATION_THRESHOLD);

    uint8_t stat = AtlasSQLite::getInstance().checkDeviceForFeatures(device.getIdentity());

    if(stat == 1){
        /* Get from db*/
        ATLAS_LOGGER_ERROR("get from local.db");

        AtlasSQLite::getInstance().selectNetwork(device.getIdentity(), (int)AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL, systemReputation);
        AtlasSQLite::getInstance().selectFeatures(device.getIdentity(), (int)AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL, systemReputation);
        
    } else if(!stat) {

        ATLAS_LOGGER_ERROR("insert into local.db");
        
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME,
                                    ATLAS_REGISTER_TIME_WEIGHT);
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS,
                                    ATLAS_KEEPALIVE_PACKETS_WEIGHT);
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS,
                                    ATLAS_VALID_PACKETS_WEIGHT);

        /* Insert into db*/
        /* Insert network in db*/
        AtlasSQLite::getInstance().insertNetwork(device.getIdentity(), (int)AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL, systemReputation);
        
        /* Insert features in db*/
        AtlasSQLite::getInstance().insertFeature(device.getIdentity(), int(AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL), int(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME), systemReputation[AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME].getSuccessfulTransactions(), ATLAS_REGISTER_TIME_WEIGHT);
        AtlasSQLite::getInstance().insertFeature(device.getIdentity(), int(AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL), int(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS), systemReputation[AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS].getSuccessfulTransactions(), ATLAS_KEEPALIVE_PACKETS_WEIGHT);
        AtlasSQLite::getInstance().insertFeature(device.getIdentity(), int(AtlasDeviceNetworkType::ATLAS_NETWORK_CONTROL), int(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS), systemReputation[AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS].getSuccessfulTransactions(), ATLAS_VALID_PACKETS_WEIGHT);
        
    } else {
        /* Error*/
        ATLAS_LOGGER_ERROR("Error when reading from local.db");
    }

    /* Add feedack for system reputation */
    std::unique_ptr<IAtlasFeedback> regFeedback(new AtlasRegistrationFeedback(device,
                                                                              ATLAS_SYSTEM_REPUTATION_INTERVAL_MS / 1000));
    feedback_[device.getIdentity()].push_back(std::move(regFeedback));

    std::unique_ptr<IAtlasFeedback> kaFeedback(new AtlasKeepaliveFeedback(device,
                                                                          ATLAS_SYSTEM_REPUTATION_INTERVAL_MS / ATLAS_KEEP_ALIVE_INTERVAL_MS));
    feedback_[device.getIdentity()].push_back(std::move(kaFeedback));

    std::unique_ptr<IAtlasFeedback> pktFeedback(new AtlasPacketsFeedback(device));
    feedback_[device.getIdentity()].push_back(std::move(pktFeedback));
}

AtlasDevice& AtlasDeviceManager::getDevice(const std::string& identity)
{
    if (devices_.find(identity) == devices_.end()){
        ATLAS_LOGGER_INFO1("New client device created with identity ", identity);
        devices_[identity] = AtlasDevice(identity, deviceCloud_);
        initSystemReputation(devices_[identity]);
    }

    return devices_[identity];
}

void AtlasDeviceManager::forEachDevice(std::function<void(AtlasDevice&)> cb)
{
    for (auto it = devices_.begin(); it != devices_.end(); ++it)
        cb(it->second); 
}

void AtlasDeviceManager::installAllPolicies()
{
    forEachDevice([] (AtlasDevice& device)
                     { 
                         if(device.getPolicy())
                             AtlasPubSubAgent::getInstance().installFirewallRule(device.getIdentity(),
                                                                                 device.getPolicy());
                     });
}

AtlasDeviceManager::~AtlasDeviceManager()
{
    /* Stop firewall statistics alarm */
    fsAlarm_.cancel();
    /* Stop system reputation alarm */
    sysRepAlarm_.cancel();
}

} // namespace atlas
