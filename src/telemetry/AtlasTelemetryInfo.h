#ifndef __ATLAS_TELEMETRY_INFO_H__
#define __ATLAS_TELEMETRY_INFO_H__

#include <unordered_map>

namespace atlas {

const std::string TELEMETRY_HOSTNAME = "hostname";
const std::string TELEMETRY_KERN_INFO = "kernel-info";

class AtlasTelemetryInfo
{

public:
    /**
    * @brief Set telemetry feature
    * @param[in] feature Telemetry feature name
    * @param[in] value Telemetry feature value
    * @return none
    */
    inline void setFeature(const std::string &feature, const std::string &value) { features_[feature] = value; }
    
    /**
    * @brief Get telemetry feature
    * @param[in] feature Telemetry feature name
    * @return Telemetry feature value
    */
    inline std::string getFeature(const std::string &feature) { return features_[feature]; }
   
    /**
    * @brief Convert telemetry info to human-readable string
    * @return String containing all the key-value telemetry feature pairs
    */ 
    std::string toString();

private:
    /* Telemetry features */
    std::unordered_map<std::string, std::string> features_;
};

} // namespace atlas

#endif /* __ATLAS_TELEMETRY_INFO_H__ */
