#include "AtlasReputation_NaiveBayes.h"
#include "../logger/AtlasLogger.h"

namespace atlas
{

float AtlasReputationNaiveBayes::computeForFeature(AtlasDeviceFeatureManager& manager, AtlasDeviceFeatureType type, int newFeedback)
{
    manager[type].addFeedbackValue(newFeedback);
    manager[type].updateTotalInteractions();
    int tmpScore = 0;

    if ( newFeedback >= manager[type].getFeedbackThreshold()){
        manager[type].updateSuccessInteractions();
        manager[type].updateCPT();
    }

    if ((manager[type].getTotalInteractions() == 0) || (manager[type].getSuccessInteractions() == 0)) {
        manager[type].updateReputation(0);
        ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForFeature: Reputation could not be calculated!");
        return 0;
    }

    float trustProb = manager[type].getSuccessInteractions() / manager[type].getTotalInteractions();
    float featureProb = manager[type].getCPT() / manager[type].getTotalInteractions();

    manager[type].updateReputation(featureProb / trustProb);
    ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForFeature: Reputation calculated successfully!");
    return (featureProb / trustProb);
}

std::vector<float> AtlasReputationNaiveBayes::computeForDevice(AtlasDeviceFeatureManager& manager, std::unordered_map<AtlasDeviceFeatureType, int>& newFeedbackMatrix)
{
    std::vector<float> tmpReputation;
    for (auto it = newFeedbackMatrix.begin(); it != newFeedbackMatrix.end(); it++) {
        tmpReputation.push_back(computeForFeature(manager, (*it).first, (*it).second));
    }

    ATLAS_LOGGER_INFO("AtlasReputationNaiveBayes_computeForDevice: Reputation calculated successfully!");
    return tmpReputation;
}

} //namespace atlas