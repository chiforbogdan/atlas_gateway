#ifndef __ATLAS_TELEMETRY_INFO_H__
#define __ATLAS_TELEMETRY_INFO_H__

#include <unordered_map>

namespace atlas {

const std::string TELEMETRY_HOSTNAME = "hostname";
const std::string TELEMETRY_KERN_INFO = "kernel-info";
const std::string TELEMETRY_SYSINFO_UPTIME = "sysinfo-uptime";
const std::string TELEMETRY_SYSINFO_TOTALRAM = "sysinfo-totalram";
const std::string TELEMETRY_SYSINFO_FREERAM = "sysinfo-freeram";
const std::string TELEMETRY_SYSINFO_SHAREDRAM = "sysinfo-sharedram";
const std::string TELEMETRY_SYSINFO_BUFFERRAM = "sysinfo-bufferram";
const std::string TELEMETRY_SYSINFO_TOTALSWAP = "sysinfo-totalswap";
const std::string TELEMETRY_SYSINFO_FREESWAP = "sysinfo-freeswap";
const std::string TELEMETRY_SYSINFO_PROCS = "sysinfo-procs";
const std::string TELEMETRY_SYSINFO_TOTALHIGH = "sysinfo-totalhigh";
const std::string TELEMETRY_SYSINFO_FREEHIGH = "sysinfo-freehigh";
const std::string TELEMETRY_SYSINFO_LOAD1 = "sysinfo-load1";
const std::string TELEMETRY_SYSINFO_LOAD5 = "sysinfo-load5";
const std::string TELEMETRY_SYSINFO_LOAD15 = "sysinfo-load15";

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
