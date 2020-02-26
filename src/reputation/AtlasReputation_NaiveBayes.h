#ifndef __ATLAS_REPUTATION_NAIVE_BAYES_H__
#define __ATLAS_REPUTATION_NAIVE_BAYES_H__

#include <unordered_map>
#include "AtlasDeviceFeatureManager.h"

namespace atlas
{
class AtlasReputationNaiveBayes
{
public:
    static float computeForFeature(AtlasDeviceFeatureManager&, AtlasDeviceFeatureType, int);
    static std::vector<float> computeForDevice(AtlasDeviceFeatureManager&, std::unordered_map<AtlasDeviceFeatureType, int>&);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_NAIVE_BAYES_H__*/