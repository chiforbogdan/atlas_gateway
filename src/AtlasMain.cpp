#include <iostream>
#include "scheduler/AtlasScheduler.h"
#include "coap/AtlasCoapServer.h"

int main(int argc, char **argv)
{
    atlas::AtlasCoapServer coapServer("127.0.0.1", "10099", atlas::ATLAS_COAP_SERVER_MODE_BOTH, "12345678");


    atlas::AtlasScheduler::getInstance().run();

    return 0;
};
