#include <iostream>
#include "AtlasPubSubAgent.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"
#include "../commands/AtlasCommand.h"

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

void AtlasPubSubAgent::getAllFirewallRules()
{

    ATLAS_LOGGER_DEBUG("Get all firewall rules for publish-subscribe agent");

    /* Fixme REMOVE THIS */
    AtlasCommandBatch cmdBatchInner, cmdBatchOuter;
    std::string clientId = "test1";
    uint16_t qos = 1;
    uint16_t pps = 4;
    uint16_t payloadLen = 5;

    qos = htons(qos);
    pps = htons(pps);
    payloadLen = htons(payloadLen);

    AtlasCommand cmd1(ATLAS_CMD_PUB_SUB_CLIENT_ID, clientId.length(), (uint8_t *)clientId.c_str());
    AtlasCommand cmd2(ATLAS_CMD_PUB_SUB_MAX_QOS, sizeof(qos), (uint8_t *)&qos);
    AtlasCommand cmd3(ATLAS_CMD_PUB_SUB_PPS, sizeof(pps), (uint8_t *)&pps);
    AtlasCommand cmd4(ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN, sizeof(payloadLen), (uint8_t *)&payloadLen);

    cmdBatchInner.addCommand(cmd1);
    cmdBatchInner.addCommand(cmd2);
    cmdBatchInner.addCommand(cmd3);
    cmdBatchInner.addCommand(cmd4);
    std::pair<const uint8_t*, size_t> inner = cmdBatchInner.getSerializedAddedCommands();

    AtlasCommand cmd5(ATLAS_CMD_PUB_SUB_INSTALL_FIREWALL_RULE, inner.second, inner.first);
    cmdBatchOuter.addCommand(cmd5);
    std::pair<const uint8_t*, size_t> outer = cmdBatchOuter.getSerializedAddedCommands();

    write(outer.first, outer.second);
    
    AtlasCommandBatch cmdBatchInner1, cmdBatchOuter1;
    clientId = "test2";
    qos = 0;
    pps = 3;
    payloadLen = 4;

    qos = htons(qos);
    pps = htons(pps);
    payloadLen = htons(payloadLen);

    AtlasCommand cmd11(ATLAS_CMD_PUB_SUB_CLIENT_ID, clientId.length(), (uint8_t *)clientId.c_str());
    AtlasCommand cmd12(ATLAS_CMD_PUB_SUB_MAX_QOS, sizeof(qos), (uint8_t *)&qos);
    AtlasCommand cmd13(ATLAS_CMD_PUB_SUB_PPS, sizeof(pps), (uint8_t *)&pps);
    AtlasCommand cmd14(ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN, sizeof(payloadLen), (uint8_t *)&payloadLen);

    cmdBatchInner1.addCommand(cmd11);
    cmdBatchInner1.addCommand(cmd12);
    cmdBatchInner1.addCommand(cmd13);
    cmdBatchInner1.addCommand(cmd14);
    std::pair<const uint8_t*, size_t> inner1 = cmdBatchInner1.getSerializedAddedCommands();

    AtlasCommand cmd15(ATLAS_CMD_PUB_SUB_INSTALL_FIREWALL_RULE, inner1.second, inner1.first);
    cmdBatchOuter1.addCommand(cmd15);
    std::pair<const uint8_t*, size_t> outer1 = cmdBatchOuter1.getSerializedAddedCommands();

    write(outer1.first, outer1.second);
}

void AtlasPubSubAgent::processCommand(size_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;

    ATLAS_LOGGER_DEBUG("Process command from publish-subscribe agent");

    /* Parse commands */
    cmdBatch.setRawCommands(data_, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_GET_ALL_PUB_SUB_FIREWALL_RULES)
            getAllFirewallRules();
    }
}

void AtlasPubSubAgent::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
    if (!error) {
        ATLAS_LOGGER_DEBUG("Reading data from publish-subscribe agent");
   
        processCommand(bytesTransferred);

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
