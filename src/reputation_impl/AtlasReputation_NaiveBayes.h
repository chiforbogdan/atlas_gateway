#ifndef __ATLAS_REPUTATION_NAIVE_BAYES_H__
#define __ATLAS_REPUTATION_NAIVE_BAYES_H__

#define ATLAS_REPUTATION_NAIVE_BAYES_DECREASE_FACTOR 0.95

#include <unordered_map>
#include "AtlasDeviceFeatureManager.h"

namespace atlas
{
class AtlasReputationNaiveBayes
{
public:

    /**
     * @brief Computes reputation for a single Feature, either a DataPlane or ControlPlane type
     * @param[in] Feature manager
     * @param[in] Feature type
     * @return Reputation value of the feature
    */
    static double computeReputationForFeature(AtlasDeviceFeatureManager&, AtlasDeviceFeatureType);

    /**
     * @brief Generic computation of reputation for a Device, (Method used in simulations)
     * @param[in] Feature manager
     * @param[in] Vector of pairs containing FeatureType, as first value, and DataPlane Feedback, as second value
     * @return Reputation value for the Device
    */
    static double computeReputation(AtlasDeviceFeatureManager&, std::vector<std::pair<AtlasDeviceFeatureType, double>>&);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_NAIVE_BAYES_H__*/