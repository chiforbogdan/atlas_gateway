#ifndef __ATLAS_DEVICE_FEATURE_H__
#define __ATLAS_DEVICE_FEATURE_H__

#include <stdio.h>
#include <vector>
#include "AtlasDeviceFeatureType.h"

namespace atlas
{
class AtlasDeviceFeature {
public:
    /**
     * @brief Default Ctor with FeatureType = GENERIC and Value = 0
     * @return none
    */
    AtlasDeviceFeature() : featureType_(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC), feedbackThreshold_(0.8), currentReputation_(1), successTrans_(0), cpt_(0) {}

    /**
     * @brief Explicit Ctor for FeatureType
     * @param[in] Specific feature from a list of FeatureTypes
     * @param[in] Initial threshold for feedback
     * @return none
    */
    AtlasDeviceFeature(AtlasDeviceFeatureType featureType, double feedbackThreshold) : featureType_(featureType), feedbackThreshold_(feedbackThreshold), currentReputation_(1), 
                                                                                        successTrans_(0), cpt_(0) {}

    /**
     * @brief Get feature type
     * @return Current feature type
    */
    AtlasDeviceFeatureType getFeatureType() { return featureType_; }

    /**
     * @brief Get feature type (const method)
     * @return Current feature type
    */
    AtlasDeviceFeatureType getFeatureType() const { return featureType_; }

    /**
     * @brief Get Feedback threshold value
     * @return Current Feedback Threshold value
    */
    double getFeedbackThreshold() { return feedbackThreshold_; }

    /**
     * @brief Set Feedback threshold value
     * @param[in] New Feedback Threshold value
     * @return none
    */
    void setFeedbackThreshold(double feedbackThreshold) { feedbackThreshold_ = feedbackThreshold; }

    /**
     * @brief Updates the current reputation value and adds the old value in the reputation history
     * @param[in] New value for reputation
     * @return none
    */
    void updateReputation(double newVal) { currentReputation_ = newVal; }

    /**
     * @brief Get the current reputation value of the feature
     * @return Reputation value
    */
    double getReputation() { return currentReputation_; }

    /**
     * @brief Updates the Conditional Probability variable
     * @return none
    */
    void updateCPT(double newVal) { cpt_ = newVal; }

    /**
     * @brief Returns the CPT value
     * @return CPT value 
    */
    double getCPT() { return cpt_; }

    /**
     * @brief Updates number of successful transactions (required by Naive Bayes component)
     * @return none
    */
    void updateSuccessfulTransactions() { ++successTrans_; }
        
    /**
     * @brief Return number of successful transactions (required by Naive Bayes component)
     * @return Number of Successful Transactions
    */
    int getSuccessfulTransactions() { return successTrans_; }

    AtlasDeviceFeature(const AtlasDeviceFeature &src) { featureType_ = src.featureType_; feedbackThreshold_ = src.feedbackThreshold_; currentReputation_ = src.currentReputation_; 
                                                        successTrans_ = src.successTrans_; cpt_ = src.cpt_; }
                                    
    AtlasDeviceFeature& operator = (const AtlasDeviceFeature& src) { featureType_ = src.featureType_; feedbackThreshold_ = src.feedbackThreshold_; currentReputation_ = src.currentReputation_; 
                                                                    successTrans_ = src.successTrans_; cpt_ = src.cpt_; return *this;}
private:
    AtlasDeviceFeatureType featureType_;
    double feedbackThreshold_;
    double currentReputation_;
    int successTrans_;
    double cpt_;

    //FIXME
    // Add history for reputation values and received feedback, if necessary
};

} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_H__*/