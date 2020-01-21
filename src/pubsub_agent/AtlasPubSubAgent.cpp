#include "AtlasPubSubAgent.h"
#include "../scheduler/AtlasScheduler.h"

namespace atlas {

const std::string ATLAS_PUB_SUB_AGENT_SOCK = "/tmp/atlas_pub_sub_agent";

void AtlasPubSubAgent::start()
{
    auto sockInfo = AtlasScheduler::getInstance().getUnixSocketServer(ATLAS_PUB_SUB_AGENT_SOCK);
}

} // namespace atlas
