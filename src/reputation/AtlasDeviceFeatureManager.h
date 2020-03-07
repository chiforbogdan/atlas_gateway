#ifndef __ATLAS_DEVICE_FEATURE_MANAGERR_H__
#define __ATLAS_DEVICE_FEATURE_MANAGERR_H__

#define ATLAS_DEVICE_FEATURE_MANAGER_DATAPLANE_WEIGHT 0.6
#define ATLAS_DEVICE_FEATURE_MANAGER_CONTROLPLANE_WEIGHT 0.4

#include "AtlasDeviceFeatureType.h"
#include "AtlasDeviceFeature.h"

namespace atlas
{
class AtlasDeviceFeatureManager
{
public:
    /**
     * @brief Add a new feature to a device or update the value of an existing feature
     * @param[in] Feature type
     * @param[in] Feature value
     * @return nonne
    */
    void addFeature(AtlasDeviceFeatureType type, double featureWeight, double feedbackThreshold);

    /**
     * @brief Remove a feature from a device
     * @param[in] Feature to be deleted identified by its type
     * @return True is the deletion succeedes. False otherwise.
    */
    bool removeFeature(AtlasDeviceFeatureType type);

    /**
     * @brief Get reputation score for a specific device feature
     * @param[in] Targeted feature
     * @return Reputation score of the feature
    */
    double getDeviceFeatureReputation(AtlasDeviceFeatureType type);

    /**
     * @brief Updates number of successful transactions (required by Naive Bayes component)
     * @return none
    */
    void updateTotalSuccessfulTransactions() { ++totalSuccessTrans_; }
        
    /**
     * @brief Return number of successful transactions (required by Naive Bayes component)
     * @return Number of Successful Transactions
    */
    int getTotalSuccessfulTransactions() { return totalSuccessTrans_; }

    /**
     * @brief Update total number of interactions (required by Naive Bayes component)
     * @return none
    */
    void updateTotalTransactions() { ++totalTrans_; }

    /**
     * @brief Returns the total number of transactions (required by Naive Bayes component)
     * @return Number of transactions
    */
    int getTotalTransactions() { return totalTrans_; }

    /**
     * @brief Updates the device reputation value
     * @param[in] New device reputation value
     * @return none
    */
    void updateDeviceReputation(double newVal) { deviceReputation_ = newVal; }

    /**
     * @brief Returns the reputation value of current device
     * @return Reputation value
    */
    double getDeviceReputation() { return deviceReputation_; }

    std::vector<AtlasDeviceFeature> getDeviceFeatures() { return features_; }

    AtlasDeviceFeatureManager() : totalSuccessTrans_(0), totalTrans_(0) {};
    ~AtlasDeviceFeatureManager() { features_.clear(); totalSuccessTrans_ = 0; totalTrans_ = 0; }

    AtlasDeviceFeature& operator [] (AtlasDeviceFeatureType type);
private:
    std::vector<AtlasDeviceFeature> features_;
    int totalSuccessTrans_;
    int totalTrans_;
    double deviceReputation_;
};
} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_MANAGER_H__*/