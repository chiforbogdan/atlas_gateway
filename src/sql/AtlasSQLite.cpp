#include "AtlasSQLite.h"
#include "../reputation_impl/AtlasDeviceFeatureType.h"
#include <boost/scope_exit.hpp>

namespace atlas {

namespace {

    const char *SQL_CREATE_TABLES = "BEGIN TRANSACTION;"\
                                    "CREATE TABLE IF NOT EXISTS Device("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "Identity TEXT NOT NULL UNIQUE," \
                                    "PSK TEXT NOT NULL );" \

                                    "CREATE TABLE IF NOT EXISTS NaiveBayesNetwork("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "NetworkTypeId INTEGER NOT NULL," \
                                    "TotalTrans INTEGER DEFAULT 0," \
                                    "TotalSuccessTrans INTEGER DEFAULT 0," \
                                    "DeviceId INTEGER NOT NULL," \
                                    "FOREIGN KEY (DeviceId) REFERENCES Device(Id));" \

                                    "CREATE TABLE IF NOT EXISTS NaiveBayesFeature("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "FeatureTypeId INTEGER NOT NULL," \
                                    "SuccessTrans INTEGER DEFAULT 0," \
                                    "Weight REAL DEFAULT 0," \
                                    "NetworkId INTEGER NOT NULL," \
                                    "FOREIGN KEY (NetworkId) REFERENCES NaiveBayesNetwork(Id));"\
                                    "COMMIT;";

    const char *SQL_INSERT_DEVICE = "INSERT INTO Device(Identity,PSK) VALUES(?,?);";
    const char *SQL_UPDATE_DEVICE = "UPDATE Device SET PSK=? WHERE Identity=?;";
    const char *SQL_GET_ID_DEVICE = "SELECT Id FROM Device WHERE Identity=?;";
    const char *SQL_SELECT_DEVICE = "SELECT PSK FROM Device WHERE Identity=?;";

    
    const char *SQL_INSERT_NETWORK = "INSERT INTO NaiveBayesNetwork(NetworkTypeId, TotalTrans, TotalSuccessTrans, DeviceId) VALUES(?,?,?,?);";
    const char *SQL_UPDATE_NETWORK = "UPDATE NaiveBayesNetwork "\
                                     "SET TotalTrans=?, TotalSuccessTrans=? "\
                                     "WHERE DeviceId IN (SELECT Id FROM Device WHERE Identity=?) AND NetworkTypeId=?;";

    const char *SQL_GET_ID_NETWORK = "SELECT NaiveBayesNetwork.Id FROM NaiveBayesNetwork "\
                                     "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                     "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?;";
    const char *SQL_GET_NETWORK = "SELECT NaiveBayesNetwork.TotalTrans, NaiveBayesNetwork.TotalSuccessTrans FROM NaiveBayesNetwork "\
                                  "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                  "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?;";

    const char *SQL_INSERT_FEATURE = "INSERT INTO NaiveBayesFeature(FeatureTypeId, SuccessTrans, Weight, NetworkId) VALUES(?,?,?,?);";
    const char *SQL_UPDATE_FEATURE = "UPDATE NaiveBayesFeature "\
                                     "SET SuccessTrans=? "\
                                     "WHERE NetworkId IN (SELECT NaiveBayesNetwork.Id FROM NaiveBayesNetwork INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?) AND FeatureTypeId=?;";


    const char *SQL_GET_FEATURE = "SELECT NaiveBayesFeature.FeatureTypeId, NaiveBayesFeature.Weight, NaiveBayesFeature.SuccessTrans FROM NaiveBayesFeature "\
                                  "INNER JOIN NaiveBayesNetwork ON NaiveBayesNetwork.Id == NaiveBayesFeature.NetworkId "\
                                  "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                  "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?;";
    const char *SQL_CHECK_FEATURE = "SELECT 1 FROM NaiveBayesFeature "\
                                    "INNER JOIN NaiveBayesNetwork ON NaiveBayesNetwork.Id == NaiveBayesFeature.NetworkId "\
                                    "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                    "WHERE Device.Identity=?;";
} // anonymous namespace

AtlasSQLite& AtlasSQLite::getInstance()
{
    static AtlasSQLite instance;

    return instance;
}

AtlasSQLite::AtlasSQLite()
{
    isConnected_ = false;
    pCon_ = nullptr;
}

AtlasSQLite::~AtlasSQLite()
{
    closeConnection();
}

void AtlasSQLite::closeConnection()
{
    if(!pCon_)
        return;

    sqlite3_close(pCon_);

    pCon_ = nullptr;

    ATLAS_LOGGER_DEBUG("Connection closed for local.db");
}

bool AtlasSQLite::isConnected()
{
    return isConnected_;
}

bool AtlasSQLite::initDB(const std::string &databasePath)
{
    bool commit = false;

    /* it will be executed either in the end or on any exception*/
    BOOST_SCOPE_EXIT(&commit, this_) {
        if(!commit) {
            sqlite3_close(this_->pCon_);
        }
    } BOOST_SCOPE_EXIT_END

    /* Open database, or create */
    if(sqlite3_open(databasePath.c_str(), &pCon_) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Cannot open " + databasePath + ", error:" + sqlite3_errmsg(pCon_));
        return false;
    }

    /* Constraints are enabled per connection*/
    if(sqlite3_db_config(pCon_, SQLITE_DBCONFIG_ENABLE_FKEY, 1, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Cannot set PRAGMA FKEY on " + databasePath + ", stmt:SQLITE_DBCONFIG_ENABLE_FKEY, error:" + sqlite3_errmsg(pCon_));
        return false;
    }

    /* Create tables if not exist*/
    if(sqlite3_exec(pCon_,SQL_CREATE_TABLES, NULL, 0, 0) != SQLITE_OK ) {
        ATLAS_LOGGER_ERROR("Cannot create tables on " + databasePath + ", stmt:SQL_CREATE_TABLES, error:" + sqlite3_errmsg(pCon_));
        return false;
    } 

    commit = true;
    return true;
}

bool AtlasSQLite::openConnection(const std::string &databasePath)
{
    isConnected_ = true;
    
    if(!initDB(databasePath))
    {
        isConnected_ = false;
        return isConnected_;
    }   

    ATLAS_LOGGER_DEBUG("Connection opened for local.db");

    return isConnected_;
}
bool AtlasSQLite::insertDevice(const std::string &identity, const  std::string &psk)
{
    sqlite3_stmt *stmt = nullptr;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    /*check unique value for identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDevice, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertDevice, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertDevice, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    sqlite3_reset(stmt);

    if(stat != SQLITE_ROW) {

        /*insert identity*/
        if(sqlite3_prepare_v2(pCon_, SQL_INSERT_DEVICE, -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDevice, stmt:SQL_INSERT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt, 2, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind, fct:insertDevice, stmt:SQL_INSERT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
	    }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step, fct:insertDevice, stmt:SQL_INSERT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
	    }

    } else {

        /*update identity*/	
        if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDevice, stmt:SQL_UPDATE_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        if (sqlite3_bind_text(stmt, 1, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind, fct:insertDevice, stmt:SQL_UPDATE_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
	        return false;
	    }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step, fct:insertDevice, stmt:SQL_UPDATE_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
	    }
    }

    return true;
}

bool AtlasSQLite::insertNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1, deviceId = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    /*get deviceId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertNetwork, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertNetwork, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertNetwork, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    deviceId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    /*insert network*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_NETWORK,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertNetwork, stmt:SQL_INSERT_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, networkTypeId) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, manager.getTotalTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 3, manager.getTotalSuccessfulTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, deviceId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertNetwork, stmt:SQL_INSERT_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertNetwork, stmt:SQL_INSERT_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    
    return true;
}

bool AtlasSQLite::insertFeature(const std::string &identity, int networkTypeId, int featureTypeId, int successTrans, double weight)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1, networkId = -1;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected_)
        return false;
	
    /*get networkId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_NETWORK,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertFeature, stmt:SQL_GET_ID_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertFeature, stmt:SQL_GET_ID_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertFeature, stmt:SQL_GET_ID_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    networkId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    /*insert feature*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_FEATURE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertFeature, stmt:SQL_INSERT_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, featureTypeId) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, successTrans) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 3, weight) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, networkId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertFeature, stmt:SQL_INSERT_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertFeature, stmt:SQL_INSERT_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    } 
	
    return true;
}

std::string AtlasSQLite::selectDevicePsk(const std::string &identity) 
{
    sqlite3_stmt *stmt = nullptr;
    std::string res;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return nullptr;

    /*select PSK for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_SELECT_DEVICE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectDevicePsk, stmt:SQL_SELECT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return nullptr;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectDevicePsk, stmt:SQL_SELECT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return nullptr;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:selectDevicePsk, stmt:SQL_SELECT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return nullptr;
    }

    if (stat == SQLITE_ROW && sqlite3_column_text(stmt, 0))
        res = std::string(reinterpret_cast<const char *> (sqlite3_column_text(stmt, 0)));

    return res;
}

bool AtlasSQLite::selectNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_NETWORK,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectNetwork, stmt:SQL_GET_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectNetwork, stmt:SQL_GET_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:selectNetwork, stmt:SQL_GET_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW){
        manager.setTotalTransactions(sqlite3_column_int(stmt, 0));
        manager.setTotalSuccessfulTransactions(sqlite3_column_int(stmt, 1));
    }

    return true;
}

bool AtlasSQLite::selectFeatures(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectFeatures, stmt:SQL_GET_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectFeatures, stmt:SQL_GET_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    for (;;)
    {
        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE){
            break;
        }

        if(stat != SQLITE_ROW){
            ATLAS_LOGGER_ERROR("Could not step, fct:selectFeatures, stmt:SQL_GET_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }
        manager.addFeature((AtlasDeviceFeatureType)sqlite3_column_int(stmt, 0), sqlite3_column_double(stmt, 1));
        manager[(AtlasDeviceFeatureType)sqlite3_column_int(stmt, 0)].setSuccessfulTransactions(sqlite3_column_int(stmt, 2));
    }
    
    return true;
}

bool AtlasSQLite::updateNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*update network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_NETWORK,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:updateNetwork, stmt:SQL_UPDATE_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, manager.getTotalTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, manager.getTotalSuccessfulTransactions()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:updateNetwork, stmt:SQL_UPDATE_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:updateNetwork, stmt:SQL_UPDATE_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    return true;
}

bool AtlasSQLite::updateFeatures(const std::string &identity, int networkTypeId,  atlas::AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*update features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:updateFeatures, stmt:SQL_UPDATE_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    for (auto &feature : manager.getDeviceFeatures())
    {
        if (sqlite3_bind_int(stmt, 1, feature.getSuccessfulTransactions()) != SQLITE_OK ||
            sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 3, networkTypeId) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 4, int(feature.getFeatureType())) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind, fct:updateFeatures, stmt:SQL_UPDATE_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE) {
            sqlite3_reset(stmt);
            continue;
        }

        if (stat != SQLITE_ROW) {
            ATLAS_LOGGER_ERROR("Could not step, fct:updateFeatures, stmt:SQL_UPDATE_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }
    }

    return true;
}

bool AtlasSQLite::checkDeviceForFeatures(const std::string &identity)
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:checkDeviceForFeatures, stmt:SQL_CHECK_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:checkDeviceForFeatures, stmt:SQL_CHECK_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:checkDeviceForFeatures, stmt:SQL_CHECK_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW){
        return true;
    }

    return false;
}

} // namespace atlas
