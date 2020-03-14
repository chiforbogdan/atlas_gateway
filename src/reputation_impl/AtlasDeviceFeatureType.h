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

    //Some demo features for system
    ATLAS_FEATURE_VALID_PACKETS = 1000,
    ATLAS_FEATURE_REGISTER_TIME,
    ATLAS_FEATURE_KEEPALIVE_PACKETS

    /*Extended with various feature of connected sensors.
    This can be upgraded to be manipulated by the administrator or through the Cloud component.*/
};

} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_TYPE_H__*/
