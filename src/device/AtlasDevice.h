#ifndef __ATLAS_DEVICE_H__
#define __ATLAS_DEVICE_H__

#include <string>

namespace atlas {

class AtlasDevice
{

public:
    /**
    * @brief Default ctor for client device
    * @return none
    */
    AtlasDevice() : identity_("") {}

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

private:
    /* IoT client identity */
    std::string identity_;

    /* IoT client pre-shared key */
    std::string psk_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_H__ */
