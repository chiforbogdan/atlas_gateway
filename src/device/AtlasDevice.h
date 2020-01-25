#ifndef __ATLAS_DEVICE_H__
#define __ATLAS_DEVICE_H__

#include <string>
#include "../telemetry/AtlasTelemetryInfo.h"

namespace atlas {

class AtlasDevice
{

public:
    /**
    * @brief Default ctor for client device
    * @return none
    */
    AtlasDevice() : identity_(""), registered_(false) {}

    /**
    * @brief Set identity for client device
    * @param[in] identity Client device identity
    * @return none
    */
    inline void setIdentity(const std::string &identity) { identity_ = identity; }

    /**
    * @brief Set PSK for client device
    * @param[in] psk Pre-shared key
    * @return none
    */
    inline void setPsk(const std::string &psk) { psk_ = psk; }

    /**
    * @brief Get client device identity
    * @return Client device identity
    */
    inline std::string getIdentity() const { return identity_; }

    /**
    * @brief Get client device pre-shared key 
    * @return Client device pre-shared key
    */
    inline std::string getPsk() const { return psk_; }

    /**
    * @brief Get client device registration time
    * @return Client device registration time
    */
    inline std::string getRegTime() const { return regTime_; }

    /**
    * @brief Indicate that a client device just registered
    * @return none
    */
    void registerNow();

    /**
    * @brief Indicate that a client device just sent a keep-alive
    * @return none
    */
    void keepAliveNow();

    /**
    * @brief Keep-alive expired timer callback. This will decrement the internal keep-alive counter
    * @return none
    */
    void keepAliveExpired();

    /**
    * @brief Get the client device registration state
    * @return True if the device is registered, false otherwise
    */
    inline bool isRegistered() const { return registered_; }

    inline AtlasTelemetryInfo& getTelemetryInfo() { return telemetryInfo_; }
private:
    /* IoT client identity */
    std::string identity_;

    /* IoT client pre-shared key */
    std::string psk_;

    /* Registration timestamp */
    std::string regTime_;

    /* Keep-alive timestamp */
    std::string keepAliveTime_;

    /* Indicates if the device is registered */
    bool registered_;

    /* Keep-alive counter. When this counter reaches 0, the device is de-registered */
    uint8_t kaCtr_;

    /* Telemetry info */
    AtlasTelemetryInfo telemetryInfo_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_H__ */
