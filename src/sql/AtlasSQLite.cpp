#include "AtlasSQLite.h"
#include "../reputation_impl/AtlasDeviceFeatureType.h"

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
                                     "WHERE NetworkId IN (SELECT Id FROM NaiveBayesNetwork INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?) AND FeatureTypeId=?;";


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
    if(pCon_)
        sqlite3_close(pCon_);

    pCon_ = nullptr;

    ATLAS_LOGGER_DEBUG("Connection closed for local.db");
}

bool AtlasSQLite::isConnected()
{
    return isConnected_;
}

bool AtlasSQLite::openConnection(const std::string &databasePath)
{
    isConnected_ = true;

    if(sqlite3_open(databasePath.c_str(), &pCon_)) {
        ATLAS_LOGGER_ERROR(sqlite3_errmsg(pCon_));
        isConnected_ = false;
        return isConnected_;
    }

    /* Create tables if not exist*/
    if(sqlite3_exec(pCon_,SQL_CREATE_TABLES, NULL, 0, &lastError_) != SQLITE_OK ) {
        ATLAS_LOGGER_ERROR(lastError_);
        sqlite3_free(lastError_);
        sqlite3_close(pCon_);
        isConnected_ = false;
        return isConnected_;
    } 

    ATLAS_LOGGER_DEBUG("Connection opened for local.db");

    return isConnected_;
}
uint8_t AtlasSQLite::insertDevice(const std::string &identity, const  std::string &psk)
{
    sqlite3_stmt *stmt = nullptr;
    
    if(!isConnected_)
        return -1;
	
    /*check unique value for identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare unique statement");
        sqlite3_finalize(stmt);
        return -1;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind text param");
	sqlite3_finalize(stmt);
	return -1;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step (execute) unique stmt");
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_reset(stmt);

    if(stat != SQLITE_ROW) {
        /*insert identity*/
        if(sqlite3_prepare_v2(pCon_, SQL_INSERT_DEVICE, -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare insert statement");
            sqlite3_finalize(stmt);
            return -1;
        }

        if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt, 2, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind text params");
            sqlite3_finalize(stmt);
            return -1;
	}

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step (execute) insert stmt");
            sqlite3_finalize(stmt);
            return -1;
	}
    } else {
        /*update identity*/	
        if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare update statement");
            sqlite3_finalize(stmt);
            return -1;
        }

        if (sqlite3_bind_text(stmt, 1, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind text params");
            sqlite3_finalize(stmt);
	    return -1;
	}

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step (execute) update stmt");
            sqlite3_finalize(stmt);
            return -1;
	}
    }
	
    sqlite3_finalize(stmt);

    return 0;
}

uint8_t AtlasSQLite::insertNetwork(const std::string &identity, int networkTypeId, int totalTrans, int totalSuccessTrans)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1, deviceId = -1;
    
    if(!isConnected_)
        return -1;
	
    /*get deviceId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare get_id_device statement");
        sqlite3_finalize(stmt);
        return -1;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind text param");
        sqlite3_finalize(stmt);
        return -1;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step (execute) get_id_device stmt");
        sqlite3_finalize(stmt);
        return -1;
    }
    deviceId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    if(stat != SQLITE_ROW) {

        ATLAS_LOGGER_ERROR("Device does not exist");
        sqlite3_finalize(stmt);
        return -1;
    } 
    else {
        /*insert network*/	
        if(sqlite3_prepare_v2(pCon_, SQL_INSERT_NETWORK,  -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare insert network statement");
            sqlite3_finalize(stmt);
            return -1;
        }

        if (sqlite3_bind_int(stmt, 1, networkTypeId) != SQLITE_OK ||
	        sqlite3_bind_int(stmt, 2, totalTrans) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 3, totalSuccessTrans) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 4, deviceId) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind text params");
            sqlite3_finalize(stmt);
            return -1;
	    }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step (execute) insert network stmt");
            sqlite3_finalize(stmt);
            return -1;
	    }
    }
	
    sqlite3_finalize(stmt);

    return 0;
}

uint8_t AtlasSQLite::insertFeature(const std::string &identity, int networkTypeId, int featureTypeId, int successTrans, double weight)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1, networkId = -1;
    ATLAS_LOGGER_ERROR("insertFeature:local.db " + std::to_string(networkTypeId) + " , " + std::to_string(featureTypeId) + " , " + std::to_string(successTrans));
    if(!isConnected_)
        return -1;
	
    /*get networkId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_NETWORK,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare get_id_network statement local.db");
        sqlite3_finalize(stmt);
        return -1;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind text param local.db");
        sqlite3_finalize(stmt);
        return -1;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step (execute) check stmt local.db");
        sqlite3_finalize(stmt);
        return -1;
    }
    networkId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    if(stat != SQLITE_ROW) {

        ATLAS_LOGGER_ERROR("Network does not exist!");
        sqlite3_finalize(stmt);
        return -1;
    } 
    else {
        /*insert feature*/	
        if(sqlite3_prepare_v2(pCon_, SQL_INSERT_FEATURE,  -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare insert feature statement local.db");
            sqlite3_finalize(stmt);
            return -1;
        }

        if (sqlite3_bind_int(stmt, 1, featureTypeId) != SQLITE_OK ||
	        sqlite3_bind_int(stmt, 2, successTrans) != SQLITE_OK ||
            sqlite3_bind_double(stmt, 3, weight) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 4, networkId) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind text params local.db");
            sqlite3_finalize(stmt);
            return -1;
	    }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step (execute) insert feature stmt local.db");
            sqlite3_finalize(stmt);
            return -1;
	    }
    }
	
    sqlite3_finalize(stmt);

    return 0;
}

std::string AtlasSQLite::selectDevicePsk(const std::string &identity) 
{
    sqlite3_stmt *stmt = nullptr;
    std::string res;

    if(!isConnected())
        return nullptr;

    /*select PSK for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_SELECT_DEVICE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare select statement");
        sqlite3_finalize(stmt);
	return nullptr;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind text param");
	sqlite3_finalize(stmt);
	return nullptr;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step (execute) select stmt");
	sqlite3_finalize(stmt);
	return nullptr;
    }

    if (stat == SQLITE_ROW && sqlite3_column_text(stmt, 0))
        res = std::string(reinterpret_cast<const char *> (sqlite3_column_text(stmt, 0)));

    sqlite3_finalize(stmt);

    return res;
}

uint8_t AtlasSQLite::selectNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;

    if(!isConnected())
        return -1;

    /*select network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_NETWORK,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare select statement");
        sqlite3_finalize(stmt);
	    return -1;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind text param");
        sqlite3_finalize(stmt);
        return -1;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step (execute) select stmt");
        sqlite3_finalize(stmt);
        return -1;
    }

    if (stat == SQLITE_ROW){
        manager.setTotalTransactions(sqlite3_column_int(stmt, 0));
        manager.setTotalSuccessfulTransactions(sqlite3_column_int(stmt, 1));
    }

    sqlite3_finalize(stmt);

    return 0;
}

uint8_t AtlasSQLite::selectFeatures(const std::string &identity, int networkTypeId,  atlas::AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;

    if(!isConnected())
        return -1;

    /*select features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare select statement");
        sqlite3_finalize(stmt);
	    return -1;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind text param");
        sqlite3_finalize(stmt);
        return -1;
    }

    for (;;)
    {
        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE)
            break;
        if (stat != SQLITE_ROW) {
            ATLAS_LOGGER_ERROR("Could not step (execute) select stmt");
            break;
        }
        manager.addFeature((AtlasDeviceFeatureType)sqlite3_column_int(stmt, 0), sqlite3_column_double(stmt, 1));
        manager[(AtlasDeviceFeatureType)sqlite3_column_int(stmt, 0)].setSuccessfulTransactions(sqlite3_column_int(stmt, 2));
    }
    
    sqlite3_finalize(stmt);

    return 0;
}

uint8_t AtlasSQLite::updateNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;

    if(!isConnected())
        return -1;

    /*update network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_NETWORK,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare update network stmt");
        sqlite3_finalize(stmt);
	    return -1;
    }

    if (sqlite3_bind_int(stmt, 1, manager.getTotalTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, manager.getTotalSuccessfulTransactions()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind text param update network stmt");
        sqlite3_finalize(stmt);
        return -1;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step (execute) update network stmt, " + std::string(sqlite3_errmsg(pCon_)));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);

    return 0;
}

uint8_t AtlasSQLite::updateFeatures(const std::string &identity, int networkTypeId,  atlas::AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;

    if(!isConnected())
        return -1;

    /*update features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare update feature stmt");
        sqlite3_finalize(stmt);
	    return -1;
    }
    for (size_t i = 0; i < manager.getDeviceFeatures().size(); i++)
    {
        if (sqlite3_bind_int(stmt, 1, manager[manager.getDeviceFeatures()[i].getFeatureType()].getSuccessfulTransactions()) != SQLITE_OK ||
            sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 3, networkTypeId) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 4, int(manager.getDeviceFeatures()[i].getFeatureType())) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind text param update feature stmt");
            sqlite3_finalize(stmt);
            return -1;
        }
        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE)
            break;
        if (stat != SQLITE_ROW) {
            ATLAS_LOGGER_ERROR("Could not step (execute) update feature stmt");
            break;
        }
    }
    
    sqlite3_finalize(stmt);

    return 0;
}


uint8_t AtlasSQLite::checkDeviceForFeatures(const std::string &identity)
{
    sqlite3_stmt *stmt = nullptr;

    if(!isConnected())
        return -1;

    /*select network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_DEBUG("Could not prepare check statement");
        sqlite3_finalize(stmt);
	    return -1;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_DEBUG("Could not bind text param");
        sqlite3_finalize(stmt);
        return -1;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_DEBUG("Could not step (execute) check stmt");
        sqlite3_finalize(stmt);
        return -1;
    }

    if (stat == SQLITE_ROW){
        return 1;
    }

    sqlite3_finalize(stmt);

    return 0;
}

} // namespace atlas
