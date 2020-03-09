#include "AtlasDeviceFeatureManager.h"
#include "../logger/AtlasLogger.h"

namespace atlas
{

void AtlasDeviceFeatureManager::addFeature(AtlasDeviceFeatureType type, double featureWeight)
{
    bool found = false;
    for (auto it = features_.begin(); it != features_.end(); it++) {
        if ((*it).getFeatureType() == type) {
            if ((*it).getWeight() == featureWeight) {
                ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: Feature already added with same value");
                found = true;
            } else {
                ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: Feature already added. Updating value");
                (*it).updateWeight(featureWeight);
                found = true;
            }                        
        }        
    }
    if (!found) {
        features_.push_back(AtlasDeviceFeature(type, featureWeight));
        ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: New feature added to device");
    }
}

bool AtlasDeviceFeatureManager::removeFeature(AtlasDeviceFeatureType type)
{      
    int index = -1;
    for (auto it = features_.cbegin(); (it != features_.cend()) && ((*it).getFeatureType() != type); it++) {
        index++;
    }

    if (index != -1) {
        features_.erase(features_.cbegin() + index);
        ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: Feature deleted successfully");         
    } else {        
        ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: No features available for device. Delete operation aborted!");
        return false;
    }
    return true;
}

double AtlasDeviceFeatureManager::getDeviceFeatureReputation(AtlasDeviceFeatureType type)
{
    for (auto tmp : features_) {
        if (tmp.getFeatureType() == type)
            return tmp.getReputation();
    }
    return 0;
}

AtlasDeviceFeature& AtlasDeviceFeatureManager::operator [] (AtlasDeviceFeatureType type) 
{ 
    int poz = 0;
    for (auto it = features_.begin(); (it != features_.end()) && ((*it).getFeatureType() != type); it++) {
        poz++;
    }  
    return features_[poz];
}

} //namespace atlas