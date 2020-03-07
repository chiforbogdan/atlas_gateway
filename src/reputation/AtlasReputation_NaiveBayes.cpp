#include "AtlasReputation_NaiveBayes.h"
#include "../logger/AtlasLogger.h"
#include <iostream>
#include <string>

namespace atlas
{
double AtlasReputationNaiveBayes::computeReputationForFeature(AtlasDeviceFeatureManager& manager, AtlasDeviceFeatureType type)
{       
    if ((manager.getTotalTransactions() == 0) || (manager.getTotalSuccessfulTransactions() == 0) || (manager[type].getSuccessfulTransactions() == 0)) {
        manager[type].updateReputation(0);
        ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForFeature: Reputation could not be calculated!");
        return 0;
    }

    double trustProb = (double)manager.getTotalSuccessfulTransactions() / (double)manager.getTotalTransactions();
    
    double featureProb = (double)manager[type].getSuccessfulTransactions() / (double)manager.getTotalSuccessfulTransactions();
    manager[type].updateCPT(featureProb);

    double reputationVal = 0;
    if ((featureProb != 0) && (trustProb != 0)) {
        reputationVal = featureProb * trustProb;
        manager[type].updateReputation(reputationVal);
    } else {
        manager[type].updateReputation(manager[type].getReputation() * ATLAS_REPUTATION_NAIVE_BAYES_DECREASE_FACTOR);
    }
    
    ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForFeature: Reputation calculated successfully!");
    return reputationVal;
}

double AtlasReputationNaiveBayes::computeReputation(AtlasDeviceFeatureManager& manager, std::vector<std::pair<AtlasDeviceFeatureType, double>>& feedbackMatrix)
{
    bool computeSuccess = true;

    manager.updateTotalTransactions();

    for (auto it = feedbackMatrix.begin(); it != feedbackMatrix.end(); it++)
    {
        if ((*it).second >= manager[(*it).first].getFeedbackThreshold()) {
            manager[(*it).first].updateSuccessfulTransactions();
        } else {
            computeSuccess = false;
        }
    }

    if (computeSuccess) {
        manager.updateTotalSuccessfulTransactions();
    }

    double featureRepVal = 0;
    for (auto it = feedbackMatrix.begin(); it != feedbackMatrix.end(); it++)
    {
        featureRepVal += computeReputationForFeature(manager, (*it).first) * manager[(*it).first].getWeight();      
    }

    return featureRepVal;
}

} //namespace atlas