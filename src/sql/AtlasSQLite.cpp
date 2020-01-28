#include "AtlasSQLite.h"
namespace {

	const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS IdentityPSK("  \
        						 "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
        						 "Identity TEXT NOT NULL UNIQUE," \
        						 "PSK TEXT NOT NULL );";
	const char *sqlInsert = "INSERT INTO IdentityPSK(Identity,PSK) VALUES(?,?);";// SELECT(?,?) WHERE NOT EXISTS (SELECT Identity,PSK FROM IdentityPSK WHERE Identity=?);";
	const char *sqlUpdate = "UPDATE IdentityPSK SET PSK=? WHERE Identity=?;";
	const char *sqlCheckIdentity = "SELECT 1 FROM IdentityPSK WHERE Identity=?;";
	const char *sqlSelect = "SELECT PSK FROM IdentityPSK WHERE Identity=?;";

}

namespace atlas {

AtlasSQLite::AtlasSQLite()
{
    isConnected_ = false;
}

AtlasSQLite::~AtlasSQLite()
{
    closeConnection();
}

void AtlasSQLite::closeConnection()
{
	if(pCon_)
		sqlite3_close(pCon_);
	ATLAS_LOGGER_DEBUG("Connection closed\n");
}

bool AtlasSQLite::isConnected()
{
    return isConnected_;
}

bool AtlasSQLite::openConnection(const std::string &databaseName, const std::string &databaseDir)
{
 	isConnected_ = true;

	std::string path = databaseDir + databaseName;

	if(sqlite3_open(path.c_str(), &pCon_))
	{
		std::string err = "Can't open database: ";
		err = err.append(sqlite3_errmsg(pCon_));
		ATLAS_LOGGER_ERROR(err);
		isConnected_ = false;
	}

	/* Create table 'IdentityPSK' if not exists */
	if(sqlite3_exec(pCon_, sqlCreateTable, NULL, 0, &lastError_) != SQLITE_OK )
	{
		std::string err = "Can't create table: ";
		err = err.append(lastError_);
		ATLAS_LOGGER_ERROR(err);
		sqlite3_free(lastError_);
		isConnected_ = false;
	} 
	
	ATLAS_LOGGER_DEBUG("Connection opened\n");

	return isConnected_;
}
uint8_t AtlasSQLite::insert(const std::string &identity, const  std::string &psk)
{
    if(!isConnected())
		return -1;

	sqlite3_stmt *stmt;

	/*check unique value for identity*/
	if(sqlite3_prepare_v2(pCon_, sqlCheckIdentity,  -1, &stmt, 0) != SQLITE_OK) 
	{
		ATLAS_LOGGER_ERROR("Could not prepare unique statement.\n");
		return -1;
  	}

	if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK)
	{
		ATLAS_LOGGER_ERROR("Could not bind text param.\n");
		return -1;
	}

	int stat = sqlite3_step(stmt);
	if (stat != SQLITE_DONE && stat != SQLITE_ROW) 
	{
		ATLAS_LOGGER_ERROR("Could not step (execute) unique stmt.\n");
		return -1;
  	}

	sqlite3_reset(stmt);

	if(stat != SQLITE_ROW)
	{
		/*insert identity*/
		if(sqlite3_prepare_v2(pCon_, sqlInsert,  -1, &stmt, 0) != SQLITE_OK) 
		{
			ATLAS_LOGGER_ERROR("Could not prepare insert statement.\n");
			return -1;
		}

		if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
			sqlite3_bind_text(stmt, 2, psk.c_str(), psk.length(),	SQLITE_STATIC) != SQLITE_OK)
		{
			ATLAS_LOGGER_ERROR("Could not bind text params.\n");
			return -1;
		}

		if (sqlite3_step(stmt) != SQLITE_DONE) 
		{
			ATLAS_LOGGER_ERROR("Could not step (execute) insert stmt.\n");
			return -1;
		}

		
		//fprintf(stdout, "%d row(s) inserted\n", sqlite3_total_changes(pCon_));
	}
	else
	{
		/*update identity*/	
		if(sqlite3_prepare_v2(pCon_, sqlUpdate,  -1, &stmt, 0) != SQLITE_OK) 
		{
			ATLAS_LOGGER_ERROR("Could not prepare update statement.\n");
			return -1;
		}

		if (sqlite3_bind_text(stmt, 1, psk.c_str(), psk.length(),	SQLITE_STATIC) != SQLITE_OK         ||
			sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) 
		{
			ATLAS_LOGGER_ERROR("Could not bind text params.\n");
			return -1;
		}

		if (sqlite3_step(stmt) != SQLITE_DONE) 
		{
			ATLAS_LOGGER_ERROR("Could not step (execute) update stmt.\n");
			return -1;
		}

		//fprintf(stdout, "%d row(s) updated\n", sqlite3_total_changes(pCon_));
	}
	sqlite3_finalize(stmt);
	return 0;
}
const unsigned char* AtlasSQLite::select(const std::string &identity) 
{
    if(!isConnected())
		return NULL;

	sqlite3_stmt *stmt;

	/*select PSK for given identity*/
	if(sqlite3_prepare_v2(pCon_, sqlSelect,  -1, &stmt, 0) != SQLITE_OK) 
	{
		ATLAS_LOGGER_ERROR("Could not prepare select statement.\n");
		return NULL;
  	}

	if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK)
	{
		ATLAS_LOGGER_ERROR("Could not bind text param.\n");
		return NULL;
	}

	int stat = sqlite3_step(stmt);
	if (stat != SQLITE_DONE && stat != SQLITE_ROW) 
	{
		ATLAS_LOGGER_ERROR("Could not step (execute) select stmt.\n");
		return NULL;
  	}

	const unsigned char *r = sqlite3_column_text(stmt, 0);
	//uint8_t len = sqlite3_column_bytes(stmt, 0);

	sqlite3_finalize(stmt);

	if(stat == SQLITE_ROW)
	{
		return r;
	}
	else
	{
		return NULL;
	}	
}
}