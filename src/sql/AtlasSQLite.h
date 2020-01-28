#ifndef __ATLAS_SQLITE_H__
#define __ATLAS_SQLITE_H__

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <string>
#include "../logger/AtlasLogger.h"

namespace atlas {

class AtlasSQLite
{

public:
    /**
    * @brief Ctor
    * @return none
    */
    AtlasSQLite();

    /**
    * @brief Open connection for an existing/new created database
    * @param[in] name database databaseName
    * @param[in] directory database databaseDir
    * @return connection state
    */
    bool openConnection(const std::string &databaseName, const std::string &databaseDir); 

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
    * @brief Execute query on database: insert, update if identity already exists
    * @param[in] identity - fist column
    * @param[in] psk - second column
    * @return none
    */
    uint8_t insert(const std::string &identity, const std::string &psk);

    /**
    * @brief Execute query on database: select, gets psk based on identity
    * @param[in] identity - fist column
    * @return psk
    */
    const unsigned char* select(const std::string &identity);

    /**
    * @brief Dtor. It disconnect opened database
    * @return none
    */
    ~AtlasSQLite();

private:

    /*status of the connection*/
    bool isConnected_;	 

    /*SQLite connection object*/
    sqlite3 *pCon_;		

    /*Last Error String*/
    char* lastError_;    	        
};
} // namespace atlas

#endif /* __ATLAS_SQLITE_H__ */
