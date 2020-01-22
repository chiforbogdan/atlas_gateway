#include <thread>
#include <boost/asio.hpp>
#include "AtlasFilter.h"

namespace atlas {

const std::string ATLAS_PUB_SUB_AGENT_SOCK = "/tmp/atlas_pub_sub_agent";

AtlasFilter::AtlasFilter() {}

AtlasFilter& AtlasFilter::getInstance()
{
    static AtlasFilter instance;

    return instance;
}

void AtlasFilter::gatewayConnect()
{
    boost::asio::io_service service;
    boost::asio::local::stream_protocol::endpoint ep(ATLAS_PUB_SUB_AGENT_SOCK);
    boost::asio::local::stream_protocol::socket sock(service);
    
    /* Connect to gateway */
    sock.connect(ep);
}

void AtlasFilter::init()
{
    std::thread t(&AtlasFilter::gatewayConnect, this);
    
    t.detach();
}

} // namespace atlas
