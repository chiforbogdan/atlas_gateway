#include "AtlasSQLite.h"

namespace atlas {

namespace {

    const char *SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS IdentityPSK("  \
        			 "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
        			 "Identity TEXT NOT NULL UNIQUE," \
        			 "PSK TEXT NOT NULL );";
    const char *SQL_INSERT = "INSERT INTO IdentityPSK(Identity,PSK) VALUES(?,?);";// SELECT(?,?) WHERE NOT EXISTS (SELECT Identity,PSK FROM IdentityPSK WHERE Identity=?);";
    const char *SQL_UPDATE = "UPDATE IdentityPSK SET PSK=? WHERE Identity=?;";
    const char *SQL_CHECK_IDENTITY = "SELECT 1 FROM IdentityPSK WHERE Identity=?;";
    const char *SQL_SELECT = "SELECT PSK FROM IdentityPSK WHERE Identity=?;";
} // anonymous namespace

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

    ATLAS_LOGGER_DEBUG("Connection closed");
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

    /* Create table 'IdentityPSK' if not exists */
    if(sqlite3_exec(pCon_,SQL_CREATE_TABLE, NULL, 0, &lastError_) != SQLITE_OK ) {
        ATLAS_LOGGER_ERROR(lastError_);
        sqlite3_free(lastError_);
        sqlite3_close(pCon_);
        isConnected_ = false;
        return isConnected_;
    } 
	
    ATLAS_LOGGER_DEBUG("Connection opened");

    return isConnected_;
}
uint8_t AtlasSQLite::insert(const std::string &identity, const  std::string &psk)
{
    sqlite3_stmt *stmt = nullptr;
    
    if(!isConnected_)
        return -1;
	
    /*check unique value for identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_IDENTITY,  -1, &stmt, 0) != SQLITE_OK) {
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
        if(sqlite3_prepare_v2(pCon_, SQL_INSERT, -1, &stmt, 0) != SQLITE_OK) {
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
        if(sqlite3_prepare_v2(pCon_, SQL_UPDATE,  -1, &stmt, 0) != SQLITE_OK) {
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

std::string AtlasSQLite::selectPsk(const std::string &identity) 
{
    sqlite3_stmt *stmt = nullptr;
    std::string res;

    if(!isConnected())
        return nullptr;

    /*select PSK for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_SELECT,-1, &stmt, 0) != SQLITE_OK) {
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

} // namespace atlas
