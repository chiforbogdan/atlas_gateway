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

    std::string errors[] = {"Generic error ", "Could not prepare ", "Could not bind ", "Could not step "};
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
    stmt_ = nullptr;
    commit_ = false;
    index_ = 0;
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
    stmt_ = nullptr;

    ATLAS_LOGGER_DEBUG("Connection closed for local.db");
}

bool AtlasSQLite::isConnected()
{
    return isConnected_;
}

bool AtlasSQLite::initDB(const std::string &databasePath)
{
    commit_ = false;

    /* it will be executed either in the end or on any exception*/
    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(sqlite3_errmsg(this_->pCon_));
            sqlite3_close(this_->pCon_);
        }
    } BOOST_SCOPE_EXIT_END

    /* Open database, or create */
    if(sqlite3_open(databasePath.c_str(), &pCon_) != SQLITE_OK) {
        return false;
    }

    /* Constraints are enabled per connection!!!*/
    if(sqlite3_db_config(pCon_, SQLITE_DBCONFIG_ENABLE_FKEY, 1, 0) != SQLITE_OK) {
        return false;
    }

    /* Create tables if not exist*/
    if(sqlite3_exec(pCon_,SQL_CREATE_TABLES, NULL, 0, 0) != SQLITE_OK ) {
        return false;
    } 

    commit_ = true;
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
    stmt_ = nullptr;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:insertDevice, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    /*check unique value for identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
        return false;
    }

    if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    int stat = sqlite3_step(stmt_);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        index_ = 3;
        return false;
    }

    sqlite3_reset(stmt_);

    if(stat != SQLITE_ROW) {

        /*insert identity*/
        if(sqlite3_prepare_v2(pCon_, SQL_INSERT_DEVICE, -1, &stmt_, 0) != SQLITE_OK) {
            index_ = 1;
            return false;
        }

        if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt_, 2, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK) {
            index_ = 2;
            return false;
	    }

        if (sqlite3_step(stmt_) != SQLITE_DONE) {
            index_ = 3;
            return false;
	    }

    } else {

        /*update identity*/	
        if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_DEVICE,  -1, &stmt_, 0) != SQLITE_OK) {
            index_ = 1;
            return false;
        }

        if (sqlite3_bind_text(stmt_, 1, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt_, 2, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
            index_ = 2;
	        return false;
	    }

        if (sqlite3_step(stmt_) != SQLITE_DONE) {
            index_ = 3;
            return false;
	    }
    }

    commit_ = true;
    return true;
}

bool AtlasSQLite::insertNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager)
{
    stmt_ = nullptr;
    int stat = -1, deviceId = -1;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:insertNetwork, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    /*get deviceId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
        return false;
    }

    if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    stat = sqlite3_step(stmt_);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        index_ = 3;
        return false;
    }
    deviceId = sqlite3_column_int(stmt_, 0);
    sqlite3_reset(stmt_);

    /*insert network*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_NETWORK,  -1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
        return false;
    }

    if (sqlite3_bind_int(stmt_, 1, networkTypeId) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 2, manager.getTotalTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 3, manager.getTotalSuccessfulTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 4, deviceId) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    if (sqlite3_step(stmt_) != SQLITE_DONE) {
        index_ = 3;
        return false;
    }
    
    commit_ = true;
    return true;
}

bool AtlasSQLite::insertFeature(const std::string &identity, int networkTypeId, int featureTypeId, int successTrans, double weight)
{
    stmt_ = nullptr;
    int stat = -1, networkId = -1;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:insertFeature, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected_)
        return false;
	
    /*get networkId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_NETWORK,  -1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
        return false;
    }

    if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 2, networkTypeId) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    stat = sqlite3_step(stmt_);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        index_ = 3;
        return false;
    }
    networkId = sqlite3_column_int(stmt_, 0);
    sqlite3_reset(stmt_);

    /*insert feature*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_FEATURE,  -1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
        return false;
    }

    if (sqlite3_bind_int(stmt_, 1, featureTypeId) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 2, successTrans) != SQLITE_OK ||
        sqlite3_bind_double(stmt_, 3, weight) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 4, networkId) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    if (sqlite3_step(stmt_) != SQLITE_DONE) {
        index_ = 3;
        return false;
    } 
	
    commit_ = true;
    return true;
}

std::string AtlasSQLite::selectDevicePsk(const std::string &identity) 
{
    stmt_ = nullptr;
    std::string res;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:selectDevicePsk, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return nullptr;

    /*select PSK for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_SELECT_DEVICE,-1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
	    return nullptr;
    }

    if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        index_ = 2;
        return nullptr;
    }

    int stat = sqlite3_step(stmt_);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        index_ = 3;
	    return nullptr;
    }

    if (stat == SQLITE_ROW && sqlite3_column_text(stmt_, 0))
        res = std::string(reinterpret_cast<const char *> (sqlite3_column_text(stmt_, 0)));

    commit_ = true;
    return res;
}

bool AtlasSQLite::selectNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    stmt_ = nullptr;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:selectNetwork, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_NETWORK,-1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
	    return false;
    }

    if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 2, networkTypeId) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    int stat = sqlite3_step(stmt_);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        index_ = 3;
        return false;
    }

    if (stat == SQLITE_ROW){
        manager.setTotalTransactions(sqlite3_column_int(stmt_, 0));
        manager.setTotalSuccessfulTransactions(sqlite3_column_int(stmt_, 1));
    }

    commit_ = true;
    return true;
}

bool AtlasSQLite::selectFeatures(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    stmt_ = nullptr;
    int stat = -1;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:selectFeatures, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_FEATURE,-1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
	    return false;
    }

    if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 2, networkTypeId) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    for (;;)
    {
        stat = sqlite3_step(stmt_);
        if (stat == SQLITE_DONE){
            break;
        }

        if(stat != SQLITE_ROW){
            index_ = 3;
            return false;
        }
        manager.addFeature((AtlasDeviceFeatureType)sqlite3_column_int(stmt_, 0), sqlite3_column_double(stmt_, 1));
        manager[(AtlasDeviceFeatureType)sqlite3_column_int(stmt_, 0)].setSuccessfulTransactions(sqlite3_column_int(stmt_, 2));
    }
    
    commit_ = true;
    return true;
}

bool AtlasSQLite::updateNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    stmt_ = nullptr;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:updateNetwork, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*update network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_NETWORK,-1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
	    return false;
    }

    if (sqlite3_bind_int(stmt_, 1, manager.getTotalTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 2, manager.getTotalSuccessfulTransactions()) != SQLITE_OK ||
        sqlite3_bind_text(stmt_, 3, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt_, 4, networkTypeId) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    int stat = sqlite3_step(stmt_);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        index_ = 3;
        return false;
    }

    commit_ = true;
    return true;
}

bool AtlasSQLite::updateFeatures(const std::string &identity, int networkTypeId,  atlas::AtlasDeviceFeatureManager &manager) 
{
    stmt_ = nullptr;
    int stat = -1;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:updateFeatures, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*update features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_FEATURE,-1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
	    return false;
    }

    for (auto &feature : manager.getDeviceFeatures())
    {
        if (sqlite3_bind_int(stmt_, 1, feature.getSuccessfulTransactions()) != SQLITE_OK ||
            sqlite3_bind_text(stmt_, 2, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int(stmt_, 3, networkTypeId) != SQLITE_OK ||
            sqlite3_bind_int(stmt_, 4, int(feature.getFeatureType())) != SQLITE_OK) {
            index_ = 2;
            return false;
        }

        stat = sqlite3_step(stmt_);
        if (stat == SQLITE_DONE) {
            sqlite3_reset(stmt_);
            continue;
        }
        if (stat != SQLITE_ROW) {
            index_ = 3;
            return false;
        }
    }

    commit_ = true;
    return true;
}

bool AtlasSQLite::checkDeviceForFeatures(const std::string &identity)
{
    stmt_ = nullptr;
    commit_ = false;
    index_ = 0;

    BOOST_SCOPE_EXIT(this_) {
        if(!this_->commit_) {
            ATLAS_LOGGER_ERROR(errors[this_->index_] + ",fct:checkDeviceForFeatures, " + sqlite3_errmsg(this_->pCon_));
        }
        sqlite3_finalize(this_->stmt_);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_FEATURE,-1, &stmt_, 0) != SQLITE_OK) {
        index_ = 1;
	    return false;
    }

    if (sqlite3_bind_text(stmt_, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        index_ = 2;
        return false;
    }

    int stat = sqlite3_step(stmt_);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        index_ = 3;
        return false;
    }

    commit_ = true;

    if (stat == SQLITE_ROW){
        return true;
    }

    return false;
}

} // namespace atlas
