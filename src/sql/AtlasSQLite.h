#ifndef __ATLAS_SQLITE_H__
#define __ATLAS_SQLITE_H__

#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include "../logger/AtlasLogger.h"
#include "../reputation_impl/AtlasDeviceFeatureManager.h"

namespace atlas {

const std::string ATLAS_DB_PATH = "local.db";

class AtlasSQLite
{

public:
    /**
    * @brief Get SQLite instance
    * @return SQLite instance
    */
    static AtlasSQLite& getInstance();

    /**
    * @brief Open connection for an existing/new created database
    * @param[in] databasePath Database path
    * @return connection state
    */
    bool openConnection(const std::string &databasePath); 

    /**
    * @brief Close connection with database
    * @return none
    */
    void closeConnection();

    /**
    * @brief Getter bConnected
    * @return bConnected
    */
    bool isConnected();

    /**
    * @brief Initialize data base
    * @param[in] databasePath Database path
    * @return true on success
    */
    bool initDB(const std::string &databasePath);

    /**
    * @brief Execute query on database: insert device
    * @param[in] identity - device identity
    * @param[in] psk - device psk
    * @return true on success, false on error
    */
    bool insertDevice(const std::string &identity, const std::string &psk);

    /**
    * @brief Execute query on database: insert network
    * @param[in] identity - device identity
    * @param[in] networkTypeId - naive bayes network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool insertNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: insert feature
    * @param[in] identity - device identity
    * @param[in] networkTypeId - naive bayes network type
    * @param[in] featureTypeId - feature type(features for control/dataPlane)
    * @param[in] successTrans - number of successful transactions
    * @return true on success, false on error
    */
    bool insertFeature(const std::string &identity, int networkTypeId, int featureTypeId, int successTrans, double weight);

    /**
    * @brief Execute query on database: select, get psk based on device identity
    * @param[in] identity Device identity
    * @return psk
    */
    std::string selectDevicePsk(const std::string &identity);

    /**
    * @brief Execute query on database: select, get network params based on device identity
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool selectNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: select, get features params based on device identity and network type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManager
    * @return true on success, false on error
    */
    bool selectFeatures(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

        /**
    * @brief Execute query on database: update network params based on device identity and network type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool updateNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: update features params based on device identity, network type and feature type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManager
    * @return true on success, false on error
    */
    bool updateFeatures(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Check if a device has related features in db
    * @param[in] identity Device identityr
    * @return 0 on success, -1 on error, 1 if exists related features
    */
    int8_t checkDeviceForFeatures(const std::string &identity);

    AtlasSQLite(const AtlasSQLite&) = delete;
    AtlasSQLite& operator=(const AtlasSQLite&) = delete;

private:

    /**
    * @brief Ctor
    * @return none
    */
    AtlasSQLite();

    /**
    * @brief Dtor. It disconnect opened database
    * @return none
    */
    ~AtlasSQLite();

    /*status of the connection*/
    bool isConnected_;	 

    /*SQLite connection object*/
    sqlite3 *pCon_;		

    /*Last Error String*/
    char* lastError_;    	        
};
} // namespace atlas

#endif /* __ATLAS_SQLITE_H__ */
