#include "AtlasReputation_NaiveBayes.h"
#include "../logger/AtlasLogger.h"
#include <iostream>
#include <string>

namespace atlas
{

double AtlasReputationNaiveBayes::computeForFeature(AtlasDeviceFeatureManager& manager, AtlasDeviceFeatureType type, double newFeedback, bool firstFeature)
{   
    if (firstFeature) 
        manager.updateTotalTransactions();

    if ( newFeedback >= manager[type].getFeedbackThreshold()) {
        manager[type].updateSuccessfulTransactions();
        if (firstFeature)
            manager.updateTotalSuccessfulTransactions();       
    }

    if ((manager.getTotalTransactions() == 0) || (manager[type].getSuccessfulTransactions() == 0)) {
        double retVal = manager[type].getReputation() * 0.95;
        manager[type].updateReputation(retVal);
        ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForFeature: Reputation could not be calculated!");
        return retVal;
    }

    double trustProb = 0;
    if ((manager.getTotalSuccessfulTransactions() != 0) && (manager.getTotalTransactions() != 0)) {
        trustProb = (double)manager.getTotalSuccessfulTransactions() / (double)manager.getTotalTransactions();
    }
    
    double featureProb = 0;
    if ((manager[type].getSuccessfulTransactions() != 0) && (manager.getTotalTransactions() != 0)) {        
        featureProb = (double)manager[type].getSuccessfulTransactions() / (double)manager.getTotalTransactions();
        manager[type].updateCPT(featureProb);
    }
    double val = 0;
    if ((featureProb != 0) && (trustProb != 0)) {
        //val = manager[type].getReputation() * (featureProb / trustProb);
        val = featureProb / trustProb;
        manager[type].updateReputation(val);
    } else {
        manager[type].updateReputation(manager[type].getReputation() * 0.95);
    }
    
    ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForFeature: Reputation calculated successfully!");
    return (val);
}

double AtlasReputationNaiveBayes::computeForDevice(AtlasDeviceFeatureManager& manager, std::unordered_map<AtlasDeviceFeatureType, double>& newFeedbackMatrix)
{
    bool firstFeature = true;
    for (auto it = newFeedbackMatrix.begin(); it != newFeedbackMatrix.end(); it++) {
        std::cout << "Feature " << (int)(*it).first << std::endl;
        computeForFeature(manager, (*it).first, (*it).second, firstFeature);
        firstFeature = false;
    }

    double retVal = 1;
    for (auto it = manager.getDeviceFeatures().begin(); it != manager.getDeviceFeatures().end(); it++) {
        retVal = retVal * ((*it).getSuccessfulTransactions() / manager.getTotalSuccessfulTransactions());
        /*Other option:
        retVal += (*it).getReputation();
        */
    }
    /*Other option (continued):
    retVal = retVal / manager.getDeviceFeatures().size();
    */
    retVal = retVal * (manager.getTotalSuccessfulTransactions() / manager.getTotalTransactions());
    ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForDevice: Reputation calculated successfully!");
    return retVal;
}

} //namespace atlas