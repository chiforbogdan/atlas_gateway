#ifndef __ATLAS_DEVICE_FEATURE_TYPE_H__
#define __ATLAS_DEVICE_FEATURE_TYPE_H__

namespace atlas
{
enum class AtlasDeviceFeatureType {
    //Some demo features for DataPlane
    ATLAS_DEVICE_FEATURE_GENERIC = 0,
    ATLAS_DEVICE_FEATURE_CO2,
    ATLAS_DEVICE_FEATURE_TEMPERATURE,
    ATLAS_DEVICE_FEATURE_LIGHT,
    ATLAS_DEVICE_FEATURE_HUMIDITY,
    ATLAS_DEVICE_FEATURE_AIR_PRESSURE,

    //Some demo features for ControlPlane
    ATLAS_DEVICE_FEATURE_UPTIME = 1000,
    ATLAS_DEVICE_FEATURE_PACKETS_ACCEPTANCY_RATE,
    ATLAS_DEVICE_FEATURE_REGISTERING_RATE,
    ATLAS_DEVICE_FEATURE_MISSED_KEEP_ALIVE_PACKETS

    /*Extended with various feature of connected sensors.
    This can be upgraded to be manipulated by the administrator or through the Cloud component.*/
};

} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_TYPE_H__*/