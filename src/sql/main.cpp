#include "AtlasSQLite.h"
#include "../logger/AtlasLogger.h"

int main(int argc, char* argv[])
{

   if(argc != 5 || strcmp(argv[1],"--identity") || strcmp(argv[3],"--psk"))
   {
      ATLAS_LOGGER_ERROR("\nValid format: >>./atlas_gateway_cli --identity 'identity1' --psk 'psk1'");
      return -1;
   }

   atlas::AtlasSQLite object;
   object.openConnection("local.db","./");
   object.insert(argv[2],argv[4]);
   return 0;
}
