#include <iostream>
#include "AtlasPubSubAgent.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

const std::string ATLAS_PUB_SUB_AGENT_SOCK = "/tmp/atlas_pub_sub_agent";

AtlasPubSubAgent::AtlasPubSubAgent() : acceptor_(AtlasScheduler::getInstance().getService()),
                                       acceptingSocket_(nullptr), connectedSocket_(nullptr)
{
    boost::asio::local::stream_protocol::endpoint ep(ATLAS_PUB_SUB_AGENT_SOCK);

    /* Remove previous binding */
    ::unlink(ATLAS_PUB_SUB_AGENT_SOCK.c_str());

    /* Bind acceptor to local unix socket */
    acceptor_.open(ep.protocol());
    acceptor_.bind(ep);
    acceptor_.listen();
}

AtlasPubSubAgent::~AtlasPubSubAgent()
{
    acceptor_.cancel();
    acceptor_.close();
    delete acceptingSocket_;
    delete connectedSocket_;
}

void AtlasPubSubAgent::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
    if (!error) {
        ATLAS_LOGGER_DEBUG("Reading data from publish-subscribe agent");
   
        connectedSocket_->async_read_some(boost::asio::buffer(data_, sizeof(data_)),
                                          boost::bind(&AtlasPubSubAgent::handleRead, this, _1, _2));
    } else
        ATLAS_LOGGER_ERROR("Error when reading data from publish-subscribe agent. Closing socket...");
}

void AtlasPubSubAgent::handleWrite(const boost::system::error_code& error)
{
    if (error)
        ATLAS_LOGGER_DEBUG("Error when writing to publish-subscribe agent. Closing socket...");
}

void AtlasPubSubAgent::start()
{
    acceptingSocket_ = new boost::asio::local::stream_protocol::socket(AtlasScheduler::getInstance().getService()); 

    acceptor_.async_accept(*acceptingSocket_, boost::bind(&AtlasPubSubAgent::handleAccept, this, _1));

    ATLAS_LOGGER_INFO("Start listenting for local publish-subscribe agents...");
}

void AtlasPubSubAgent::handleAccept(const boost::system::error_code& error)
{
    ATLAS_LOGGER_DEBUG("Connection handler for publish-subscribe agent called");

    if (!error) {
        ATLAS_LOGGER_DEBUG("Accepting connection from publish-subscribe agent");
        /* Allow only one connection from agent, so close the existing one */
        delete connectedSocket_;
        connectedSocket_ = acceptingSocket_;
        
        connectedSocket_->async_read_some(boost::asio::buffer(data_, sizeof(data_)),
                                          boost::bind(&AtlasPubSubAgent::handleRead, this, _1, _2));

    } else {
        std::cout << error.value() << '\n';
        std::cout << error.category().name() << '\n';
        ATLAS_LOGGER_DEBUG("Error in accepting connection from publish-subscribe agent ");
        delete acceptingSocket_;
        acceptingSocket_ = nullptr;
    }

    ATLAS_LOGGER_DEBUG("Start listening for a new connection...");

    start();
}

void AtlasPubSubAgent::write(const uint8_t *buf, size_t bufLen)
{
    if (!buf || !bufLen)
        return;

    ATLAS_LOGGER_DEBUG("Writing data to publish-subscribe agent.");

    if (connectedSocket_)
        boost::asio::async_write(*connectedSocket_, boost::asio::buffer(buf, bufLen),
                                 boost::bind(&AtlasPubSubAgent::handleWrite, this, _1));
}

} // namespace atlas
