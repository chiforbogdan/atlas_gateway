#include "../AtlasSQLite.h"
#include "../../logger/AtlasLogger.h"

#define ATLAS_DB_PATH "local.db"

static void
print_usage()
{
    printf("atlas_gateway_cli --identity <identity> --psk <psk>\n");
}

int main(int argc, char** argv)
{

   if(argc != 5 || strcmp(argv[1],"--identity") || strcmp(argv[3],"--psk"))
   {
      print_usage();
      return -1;
   }

   if(!atlas::AtlasSQLite::getInstance().openConnection(ATLAS_DB_PATH)){
      ATLAS_LOGGER_ERROR("Error opening database!");
      return -1;
   }
   atlas::AtlasSQLite::getInstance().insertDevice(argv[2],argv[4]);
   atlas::AtlasSQLite::getInstance().closeConnection();

   return 0;
}
