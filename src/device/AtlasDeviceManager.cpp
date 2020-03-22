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

/* Reputation feature weights (importance) for system network */
const double ATLAS_VALID_PACKETS_WEIGHT = 0.3;
const double ATLAS_REGISTER_TIME_WEIGHT = 0.4;
const double ATLAS_KEEPALIVE_PACKETS_WEIGHT = 0.3;

/* Reputation feature weights (importance) for data network */
const double ATLAS_SENSOR_WEIGHT = 0.7;
const double ATLAS_RESP_TIME_WEIGHT = 0.3;

/* System reputation threshold value */
const double ATLAS_SYSTEM_REPUTATION_THRESHOLD = 0.8;
/* Data reputation threshold value */
const double ATLAS_DATA_REPUTATION_THRESHOLD = 0.8;

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
                         if(device.getPolicy()) {
                             AtlasPubSubAgent::getInstance().getFirewallRuleStats((device.getPolicy()->getClientId()));
                            
                             /* update in db previous stats sample */
                             bool result = AtlasSQLite::getInstance().updateStats(device.getIdentity(),
                                                                                  *(device.getFirewallStats()));
                             if(!result)
                                 ATLAS_LOGGER_ERROR("Uncommited update on statistics data");
                         }
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
    double repVal = AtlasReputationNaiveBayes::computeReputation(device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM),
                                                                 feedbackMatrix);
    ATLAS_LOGGER_INFO("System reputation for device with identity " + device.getIdentity() + " is " + std::to_string(repVal));
    device.syncReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM);

    /* Update into db*/
    bool result = AtlasSQLite::getInstance().updateBayesParams(device.getIdentity(), 
                                                               (int) AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM, 
                                                               device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM));
    if(!result)
        ATLAS_LOGGER_ERROR("Uncommited update on naiveBayes params for System");
}

void AtlasDeviceManager::sysRepAlarmCallback()
{
    ATLAS_LOGGER_INFO("System reputation alarm callback");

    forEachDevice([this] (AtlasDevice& device) 
                         {     
                            subAlarmCallback(device);
                         });
}

void AtlasDeviceManager::updateDataReputation(const std::string &identity,
                                              AtlasDeviceNetworkType networkType,
                                              std::vector<std::pair<AtlasDeviceFeatureType, double>> &feedbackMatrix)
{
    if (devices_.find(identity) == devices_.end()) {
        ATLAS_LOGGER_ERROR("Cannot update data reputation network for un-registered devices");
        return;
    }

    if (networkType <= AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_START ||
        networkType >= AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_MAX) {
        ATLAS_LOGGER_ERROR("Invalid data reputation network type");
        return;
    }

    if (feedbackMatrix.empty()) {
        ATLAS_LOGGER_INFO("Data reputation network cannot be updated because of empty feedback matrix");
        return;
    }

    AtlasDevice &device = devices_[identity];
    if (!device.hasReputation(networkType)) {
        ATLAS_LOGGER_ERROR("Skip data reputation update: device with identity " + identity + 
                           " does not have the required network type");
        return;
    }
    
    AtlasDeviceFeatureManager &dataReputation = device.getReputation(networkType);

    /* Compute system reputation using naive-bayes */
    double repVal = AtlasReputationNaiveBayes::computeReputation(dataReputation, feedbackMatrix);
    ATLAS_LOGGER_INFO("Data reputation for device with identity " + device.getIdentity() +
                      " is " + std::to_string(repVal));
    std::cout << "Data reputation for device with identity " + device.getIdentity() + " is " + std::to_string(repVal) << std::endl;
    for (auto &elem : feedbackMatrix)
        std::cout << " " << elem.second << std::endl;

    device.syncReputation(networkType);

    /* Update into db*/
    bool result = AtlasSQLite::getInstance().updateBayesParams(device.getIdentity(),
                                                               (int) networkType,
                                                               dataReputation);
    if(!result)
        ATLAS_LOGGER_ERROR("Uncommited update on naiveBayes params for System");
}


void AtlasDeviceManager::initSystemReputation(AtlasDevice &device)
{
    bool result;

    /* Add default features for the system reputation */
    AtlasDeviceFeatureManager &systemReputation = device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM);
    
    systemReputation.updateFeedbackThreshold(ATLAS_SYSTEM_REPUTATION_THRESHOLD);

    if (AtlasSQLite::getInstance().checkDeviceForFeatures(device.getIdentity())) {
        /* Get from db*/
        ATLAS_LOGGER_INFO("Get data from local.db");
        result = AtlasSQLite::getInstance().selectBayesParams(device.getIdentity(), 
                                                             (int)AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM,
                                                             systemReputation);
        if (!result)
            ATLAS_LOGGER_ERROR("Uncommited select on naiveBayes params data");
    } else {
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME,
                                    ATLAS_REGISTER_TIME_WEIGHT);
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS,
                                    ATLAS_KEEPALIVE_PACKETS_WEIGHT);
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS,
                                    ATLAS_VALID_PACKETS_WEIGHT);

        /* Insert into db*/
        ATLAS_LOGGER_INFO("Insert data into local.db");
        result = AtlasSQLite::getInstance().insertBayesParams(device.getIdentity(),
                                                              (int) AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM,
                                                              systemReputation);
        if(!result)
            ATLAS_LOGGER_ERROR("Uncommited insert for naiveBayes params data");
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

void AtlasDeviceManager::initSystemStatistics(AtlasDevice &device)
{
    /*Get stats from DB if exists*/
    bool result = AtlasSQLite::getInstance().checkDeviceForStats(device.getIdentity());
    if(result) {
        /* select from db */   
        result = AtlasSQLite::getInstance().selectStats(device.getIdentity(), *(device.getFirewallStats()));
        if(!result)
            ATLAS_LOGGER_ERROR("Uncommited select on statistics data");
    } else {
        /* insert into db */
        result = AtlasSQLite::getInstance().insertStats(device.getIdentity(), *(device.getFirewallStats()));
        if(!result)
            ATLAS_LOGGER_ERROR("Uncommited insert on statistics data");
    }
}

void AtlasDeviceManager::initDataReputation(AtlasDevice &device)
{
    /* Add default features for the data reputation */
    AtlasDeviceFeatureManager &dataReputation = device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE);
    
    dataReputation.updateFeedbackThreshold(ATLAS_DATA_REPUTATION_THRESHOLD);
     
    dataReputation.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_SENSOR, ATLAS_SENSOR_WEIGHT);
    dataReputation.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_RESP_TIME, ATLAS_RESP_TIME_WEIGHT);

    /* Insert into db*/
    ATLAS_LOGGER_INFO("Insert data into local.db");
    bool result = AtlasSQLite::getInstance().insertBayesParams(device.getIdentity(),
                                                               (int) AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE,
                                                               dataReputation);
    if(!result)
        ATLAS_LOGGER_ERROR("Uncommited insert for naiveBayes params data");
}

AtlasDevice& AtlasDeviceManager::getDevice(const std::string& identity)
{
    if (devices_.find(identity) == devices_.end()) {
        // TODO when a device is created check if the identity exists in the database
        ATLAS_LOGGER_INFO1("New client device created with identity ", identity);
        devices_[identity] = AtlasDevice(identity, deviceCloud_);
        initSystemReputation(devices_[identity]);
        initDataReputation(devices_[identity]);
        initSystemStatistics(devices_[identity]);
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
