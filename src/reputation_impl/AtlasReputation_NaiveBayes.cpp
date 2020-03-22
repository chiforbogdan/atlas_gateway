#include "AtlasReputation_NaiveBayes.h"
#include "../logger/AtlasLogger.h"
#include <iostream>
#include <string>

namespace atlas
{
double AtlasReputationNaiveBayes::computeReputationForFeature(AtlasDeviceFeatureManager& manager, AtlasDeviceFeatureType type)
{       
    if (manager.getTotalTransactions() == 0 || manager.getTotalSuccessfulTransactions() == 0 || manager[type].getSuccessfulTransactions() == 0) {
        std::string tmpInfo = "AtlasReputationNaiveBayes_computeForFeature: Reputation could not be calculated for feature no. " + std::to_string((int)type) + " !";
        ATLAS_LOGGER_INFO(tmpInfo.c_str());
        return 0;
    }    
    
    double featureProb = (double)manager[type].getSuccessfulTransactions() / (double)manager.getTotalSuccessfulTransactions();
    
    std::string tmpInfo = "AtlasReputationNaiveBayes_computeForFeature: Reputation calculated successfully for feature no. " + std::to_string((int)type) + " !";
    ATLAS_LOGGER_INFO(tmpInfo.c_str());
    return featureProb;
}

double AtlasReputationNaiveBayes::computeReputation(AtlasDeviceFeatureManager& manager, std::vector<std::pair<AtlasDeviceFeatureType, double>>& feedbackMatrix)
{
    manager.updateTotalTransactions();
    double satisfactionScore = 0;

    //compute feedback per device 
    for (auto it = feedbackMatrix.begin(); it != feedbackMatrix.end(); it++)
        satisfactionScore += (*it).second * manager[(*it).first].getWeight();

    //check if feedback per device is >= feedback threshold
    if (satisfactionScore >= manager.getFeedbackThreshold()) {
        manager.updateTotalSuccessfulTransactions();

        //update success transactions for each feature, if the weighted feedback for each one is >= weighted threshold
        double weightedFeatureFeadback = 0, weightedFeatureFeedbackThreshold = 0;
        for (auto it = feedbackMatrix.begin(); it != feedbackMatrix.end(); it++) {
            weightedFeatureFeadback = (*it).second * manager[(*it).first].getWeight();
            weightedFeatureFeedbackThreshold = manager.getFeedbackThreshold() * manager[(*it).first].getWeight();

            if (weightedFeatureFeadback >= weightedFeatureFeedbackThreshold) {
                manager[(*it).first].updateSuccessfulTransactions();
            }
        }
    }

    //compute reputation for device
    double repVal = (double)manager.getTotalSuccessfulTransactions() / (double)manager.getTotalTransactions(); 
    for (auto it = feedbackMatrix.begin(); it != feedbackMatrix.end(); it++)
        repVal *= computeReputationForFeature(manager, (*it).first);      

    manager.updateReputation(repVal);

    return repVal;
}

} //namespace atlas
