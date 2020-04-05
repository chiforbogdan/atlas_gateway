#include "../AtlasSQLite.h"
#include "../../logger/AtlasLogger.h"
#include <boost/regex.hpp>

#define ATLAS_DB_PATH "local.db"

static bool validate_uuid(const std::string& s)
{
   static const 
   boost::regex e("[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89aAbB][a-f0-9]{3}-[a-f0-9]{12}");
   return regex_match(s, e);
}

static void
print_usage()
{
    printf("Invalid arguments.\n"\
           "Usage: atlas_gateway_cli --identity <identity> --psk <psk>\n"
           "Identity must be an UUID value, PSK must be a random 44 bytes value\n");
}

int main(int argc, char** argv)
{

   if(argc != 5 || strcmp(argv[1],"--identity") || strcmp(argv[3],"--psk")
                || !validate_uuid(argv[2]) || strlen(argv[4]) != 44) {
      print_usage();
      return -1;
   }
   
   if(!atlas::AtlasSQLite::getInstance().openConnection(ATLAS_DB_PATH)) {
      ATLAS_LOGGER_ERROR("Error opening database!");
      return -1;
   }

   atlas::AtlasSQLite::getInstance().insertDevice(argv[2],argv[4]);
   atlas::AtlasSQLite::getInstance().closeConnection();

   return 0;
}
