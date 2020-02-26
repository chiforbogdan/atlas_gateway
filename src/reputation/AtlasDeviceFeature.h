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
    AtlasDeviceFeature() : featureType_(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC), feedbackThreshold_(0), currentReputation_(0), 
                            totalInteractions_(0), successInteractions_(0), cpt_(0) {}

    /**
     * @brief Explicit Ctor for FeatureType
     * @param[in] Specific feature from a list of FeatureTypes
     * @param[in] Initial threshold for feedback
     * @return none
    */
    AtlasDeviceFeature(AtlasDeviceFeatureType featureType, int feedbackValue) : featureType_(featureType), feedbackThreshold_(feedbackValue), currentReputation_(0),
                            totalInteractions_(0), successInteractions_(0), cpt_(0) {}

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
     * @return Current Feedback value
    */
    int getFeedbackThreshold() { return feedbackThreshold_; }

    /**
     * @brief Set Feedback threshold value
     * @param[in] New Feedback value
     * @return none
    */
    void setFeedbackThreshold(int feedbackValue) { feedbackThreshold_ = feedbackThreshold_; }

    /**
     * @brief Updates the current reputation value and adds the old value in the reputation history
     * @param[in] New value for reputation
     * @return none
    */
    void updateReputation(float newVal) { currentReputation_ = newVal; reputationHistory_.push_back(newVal); }

    /**
     * @brief Get the current reputation value of the feature
     * @return Reputation value
    */
    int getReputation() { return currentReputation_; }

    /**
     * @brief Returns the reputation history for the current feature
     * @return Vector with all reputation values
    */
    std::vector<float> getReputationHistory() { return reputationHistory_;}

    /**
     * @brief Add a new Feedback value for current feature
     * @param[in] New Feedbackk value
     * @return none
    */
    void addFeedbackValue(int val) { feedbackHistory_.push_back(val); }

    /**
     * @brief Update total number of interactions (required by Naive Bayes component)
     * @return none
    */
    void updateTotalInteractions() { totalInteractions_++; }

    /**
     * @brief Returns the total number of interactions (required by Naive Bayes component)
     * @return Number of interactions
    */
    int getTotalInteractions() { return totalInteractions_; }

    /**
     * @brief Updates number of successful interactions (required by Naive Bayes component)
     * @return none
    */
    void updateSuccessInteractions() { successInteractions_++; }

    /**
     * @brief Return number of successful intearctions (required by Naive Bayes component)
     * @return none
    */
    int getSuccessInteractions() { return successInteractions_; }

    /**
     * @brief Updates the Conditional Probability variable
     * @return none
    */
    void updateCPT() { cpt_++; }

    /**
     * @brief Returns the CPT value
     * @return CPT value 
    */
    int getCPT() { return cpt_; }


    AtlasDeviceFeature(const AtlasDeviceFeature &src) { featureType_ = src.featureType_; feedbackThreshold_ = src.feedbackThreshold_; currentReputation_ = src.currentReputation_; 
                                    reputationHistory_ = src.reputationHistory_; totalInteractions_ = src.totalInteractions_; successInteractions_ = src.successInteractions_;
                                    cpt_ = src.cpt_; reputationHistory_ = src.reputationHistory_; feedbackHistory_ = src.feedbackHistory_; }
                                    
    AtlasDeviceFeature& operator = (const AtlasDeviceFeature& src) { featureType_ = src.featureType_; feedbackThreshold_ = src.feedbackThreshold_; currentReputation_ = src.currentReputation_; 
                                    reputationHistory_ = src.reputationHistory_; totalInteractions_ = src.totalInteractions_; successInteractions_ = src.successInteractions_;
                                    cpt_ = src.cpt_; reputationHistory_ = src.reputationHistory_; feedbackHistory_ = src.feedbackHistory_; }
private:
    AtlasDeviceFeatureType featureType_;
    int feedbackThreshold_;
    float currentReputation_;
    std::vector<float> reputationHistory_;
    std::vector<int> feedbackHistory_;
    int totalInteractions_;
    int successInteractions_;
    int cpt_;
};

} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_H__*/