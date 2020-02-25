#ifndef __ATLAS_DEVICE_H__
#define __ATLAS_DEVICE_H__

#include <string>
#include <memory>
#include "../telemetry/AtlasTelemetryInfo.h"
#include "../telemetry/AtlasAlert.h"

namespace atlas {

const std::string ATLAS_COAPS_SCHEME = "coaps://";

class AtlasDevice
{

public:
    /**
    * @brief Default ctor for client device
    * @return none
    */
    AtlasDevice();

    /**
    * @brief Ctor for client device
    * @param[in] identity Client device identity
    * @return none
    */
    AtlasDevice(const std::string &identity);

    /**
    * @brief Set PSK for client device
    * @param[in] psk Pre-shared key
    * @return none
    */
    inline void setPsk(const std::string &psk) { psk_ = psk; }


    /**
    * @brief Set IP for client device
    * @param[in] ip address
    * @return none
    */
    inline void setIpPort(const std::string &ipPort) { ipPort_ = ipPort; }

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
    * @brief Get client device pre-shared key as char array
    * @return Client device pre-shared key as char array
    */
    inline const char *getPskAsCharArray() const { return psk_.c_str(); }

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

    /**
    * @brief Get telemetry info
    * @return Reference to telemetry info
    */
    inline AtlasTelemetryInfo& getTelemetryInfo() { return telemetryInfo_; }

    /**
    * @brief Push all telemetry alerts on client device
    * @return none
    */
    void pushAlerts();

    /**
    *@brief Get device URL
    * @return Client device URL
    */
    inline std::string getUrl() const { return ATLAS_COAPS_SCHEME + ipPort_; }

private:
    /**
    * @brief Install default telemetry alerts
    * @return none
    */
    void installDefaultAlerts();

    /* IoT client identity */
    std::string identity_;

    /* IoT client pre-shared key */
    std::string psk_;

    /* IoT client ip adrress & port */
    std::string ipPort_;

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

    /* Telemetry push alerts */
    std::unordered_map<std::string, std::unique_ptr<AtlasAlert> > pushAlerts_;

    /* Telemetry threshold alerts */
    std::unordered_map<std::string, std::unique_ptr<AtlasAlert> > thresholdAlerts_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_H__ */
