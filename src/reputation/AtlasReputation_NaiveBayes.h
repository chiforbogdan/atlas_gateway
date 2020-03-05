#ifndef __ATLAS_REPUTATION_NAIVE_BAYES_H__
#define __ATLAS_REPUTATION_NAIVE_BAYES_H__

#include <unordered_map>
#include "AtlasDeviceFeatureManager.h"

namespace atlas
{
class AtlasReputationNaiveBayes
{
public:
    static double computeForFeature(AtlasDeviceFeatureManager&, AtlasDeviceFeatureType, double, bool firstFeature = true);
    static double computeForDevice(AtlasDeviceFeatureManager&, std::unordered_map<AtlasDeviceFeatureType, double>&);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_NAIVE_BAYES_H__*/