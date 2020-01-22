#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "AtlasFilter.h"
#include "../logger/AtlasLogger.h"
#include "../../commands/AtlasCommandType.h"
#include "../../commands/AtlasCommand.h"
#include "../../commands/AtlasCommandBatch.h"

namespace atlas {

const std::string ATLAS_PUB_SUB_AGENT_SOCK = "/tmp/atlas_pub_sub_agent";
const int ATLAS_SOCKET_RECONNECT_RETRY_SEC = 10;

AtlasFilter::AtlasFilter() : work_(ioService_), socket_(nullptr) {}

AtlasFilter& AtlasFilter::getInstance()
{
    static AtlasFilter instance;

    return instance;
}

void AtlasFilter::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
    if (!error) {
        ATLAS_LOGGER_DEBUG("Reading data from gateway");

        socket_->async_read_some(boost::asio::buffer(data_, sizeof(data_)),
                                 boost::bind(&AtlasFilter::handleRead, this, _1, _2));
    } else
        ATLAS_LOGGER_ERROR("Error when reading data from gateway");
}

void AtlasFilter::handleWrite(const boost::system::error_code& error)
{
    if (error)
        ATLAS_LOGGER_DEBUG("Error when writing to gateway");
}


void AtlasFilter::gatewayConnect()
{
    while(true) {
        try {
            boost::asio::local::stream_protocol::endpoint ep(ATLAS_PUB_SUB_AGENT_SOCK);
            delete socket_;
            socket_ = new boost::asio::local::stream_protocol::socket(ioService_);
    
            /* Connect to gateway */
            socket_->connect(ep);
        
            socket_->async_read_some(boost::asio::buffer(data_, sizeof(data_)),
                                              boost::bind(&AtlasFilter::handleRead, this, _1, _2));

            /* Get all firewall rules */
            AtlasCommandBatch cmdBatch;
            AtlasCommand getFwRulesCmd(ATLAS_CMD_GET_ALL_PUB_SUB_FIREWALL_RULES, 0, nullptr);
            cmdBatch.addCommand(getFwRulesCmd);
            std::pair<const uint8_t*, size_t> serializedCmd = cmdBatch.getSerializedAddedCommands();

            boost::asio::async_write(*socket_,
                                     boost::asio::buffer(serializedCmd.first, serializedCmd.second),
                                     boost::bind(&AtlasFilter::handleWrite, this, _1));

            ATLAS_LOGGER_DEBUG("Publish-subscribe agent is connected to the gateway");
            break;
        } catch (std::exception &e) {
            ATLAS_LOGGER_DEBUG("An exception occured when trying to connect to the gateway: %s", e.what());
            std::this_thread::sleep_for (std::chrono::seconds(ATLAS_SOCKET_RECONNECT_RETRY_SEC));
        }
    }
}

void AtlasFilter::gatewayLoop()
{
    ATLAS_LOGGER_DEBUG("Starting gateway connection main loop...");
    
    gatewayConnect();

    ioService_.run();
}

void AtlasFilter::init()
{
    std::thread t(&AtlasFilter::gatewayLoop, this);
    
    t.detach();
}

} // namespace atlas
