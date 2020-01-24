#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <utility>
#include <arpa/inet.h>
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

void AtlasFilter::writeFirewallRuleStats(const std::string &clientId, uint32_t droppedPkts, uint32_t passedPkts)
{
    AtlasCommandBatch cmdBatchInner;
    AtlasCommandBatch cmdBatchOuter;

    /* Inner commands */
    droppedPkts = htonl(droppedPkts);
    passedPkts = htonl(passedPkts);

    AtlasCommand cmdClientId(ATLAS_CMD_PUB_SUB_CLIENT_ID, clientId.length(), (uint8_t *) clientId.c_str());
    AtlasCommand cmdDroppedPkts(ATLAS_CMD_PUB_SUB_PKT_DROP, sizeof(uint32_t), (uint8_t *) &droppedPkts);
    AtlasCommand cmdPassedPkts(ATLAS_CMD_PUB_SUB_PKT_PASS, sizeof(uint32_t), (uint8_t *) &passedPkts);
    
    cmdBatchInner.addCommand(cmdClientId);
    cmdBatchInner.addCommand(cmdDroppedPkts);
    cmdBatchInner.addCommand(cmdPassedPkts);

    std::pair<const uint8_t*, size_t> innerCmd = cmdBatchInner.getSerializedAddedCommands();

    /* Outer commands */
    AtlasCommand cmdPutStatRule(ATLAS_CMD_PUB_SUB_PUT_STAT_FIREWALL_RULE, innerCmd.second, innerCmd.first);

    cmdBatchOuter.addCommand(cmdPutStatRule);

    std::pair<const uint8_t*, size_t> outerCmd = cmdBatchOuter.getSerializedAddedCommands();

    /* Write command to gateway */
    boost::asio::async_write(*socket_,
                             boost::asio::buffer(outerCmd.first, outerCmd.second),
                             boost::bind(&AtlasFilter::handleWrite, this, _1));

    ATLAS_LOGGER_DEBUG("Firewall rule statistics sent by agent");
}

void AtlasFilter::getFirewallRuleStats(const uint8_t *cmdBuf, uint16_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    std::string clientId = "";
    uint32_t droppedPkts = 0;
    uint32_t passedPkts = 0;

    ATLAS_LOGGER_DEBUG("Get firewall rule statistics from agent");
    
    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuf, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_CLIENT_ID)
            clientId.assign((char *)cmd.getVal(), cmd.getLen());
    }
    
    if (clientId != "") {
        /* Remove firewall rule */
        mutex_.lock();
        droppedPkts = rules_[clientId].getStatDroppedPkt();
        passedPkts = rules_[clientId].getStatPassedPkt();
        mutex_.unlock();
    } else
        ATLAS_LOGGER_ERROR("Empty client id in firewall get statistics command");

    writeFirewallRuleStats(clientId, droppedPkts, passedPkts);
} 

void AtlasFilter::removeFirewallRule(const uint8_t *cmdBuf, uint16_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    std::string clientId = "";

    ATLAS_LOGGER_DEBUG("Remove firewall rule from agent");
    
    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuf, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds)
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_CLIENT_ID)
            clientId.assign((char *)cmd.getVal(), cmd.getLen());
 
    if (clientId == "") {    
        ATLAS_LOGGER_ERROR("Empty client id in firewall remove command. Dropping command...");
        return;
    }

    /* Remove firewall rule */
    mutex_.lock();
    rules_.erase(clientId);
    mutex_.unlock();

    ATLAS_LOGGER_INFO("Firewall rule for client id %s was removed", clientId.c_str());
} 

void AtlasFilter::installFirewallRule(const uint8_t *cmdBuf, uint16_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    std::string clientId = "";
    uint16_t maxQos;
    uint16_t pps;
    uint16_t maxPayloadLen;
    bool qosFound = false;
    bool ppsFound = false;
    bool payloadLenFound = false;

    ATLAS_LOGGER_DEBUG("Install firewall rule from gateway");
    
    /* Parse commands */
    cmdBatch.setRawCommands(cmdBuf, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_CLIENT_ID) {
            clientId.assign((char *)cmd.getVal(), cmd.getLen());
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_MAX_QOS && cmd.getLen() == sizeof(uint16_t)) {
            memcpy(&maxQos, cmd.getVal(), sizeof(uint16_t));
            maxQos = ntohs(maxQos);
            qosFound = true;
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_PPS && cmd.getLen() == sizeof(uint16_t)) {
            memcpy(&pps, cmd.getVal(), sizeof(uint16_t));
            pps = ntohs(pps);
            ppsFound = true;      
        } else if (cmd.getType() == ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN && cmd.getLen() == sizeof(uint16_t)) {
            memcpy(&maxPayloadLen, cmd.getVal(), sizeof(uint16_t));
            maxPayloadLen = ntohs(maxPayloadLen);
            payloadLenFound = true;
        }
    }

    if (clientId == "") {    
        ATLAS_LOGGER_ERROR("Empty client id in firewall install command. Dropping command...");
        return;
    }

    if (!qosFound) {    
        ATLAS_LOGGER_ERROR("Empty QoS in firewall install command. Dropping command...");
        return;
    }

    if (!ppsFound) {    
        ATLAS_LOGGER_ERROR("Empty PPS in firewall install command. Dropping command...");
        return;
    }

    if (!payloadLenFound) {    
        ATLAS_LOGGER_ERROR("Empty payload length in firewall install command. Dropping command...");
        return;
    }

    /* Install firewall rule */
    mutex_.lock();
    rules_[clientId] = AtlasPacketPolicer(maxQos, pps, maxPayloadLen);
    mutex_.unlock();
    
    ATLAS_LOGGER_INFO("Installed firewall rule for client id %s, Max QoS: %d, PPS: %d, Max payload length: %d",
                      clientId.c_str(), maxQos, pps, maxPayloadLen);
}

void AtlasFilter::processCommand(size_t cmdLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmds;
    
    ATLAS_LOGGER_DEBUG("Process command from gateway");
    
    /* Parse commands */
    cmdBatch.setRawCommands(data_, cmdLen);
    cmds = cmdBatch.getParsedCommands();

    for (const AtlasCommand &cmd : cmds) {
        if (cmd.getType() == ATLAS_CMD_PUB_SUB_INSTALL_FIREWALL_RULE)
            installFirewallRule(cmd.getVal(), cmd.getLen());
        else if (cmd.getType() == ATLAS_CMD_PUB_SUB_REMOVE_FIREWALL_RULE)
            removeFirewallRule(cmd.getVal(), cmd.getLen());
        else if (cmd.getType() == ATLAS_CMD_PUB_SUB_GET_STAT_FIREWALL_RULE)
            getFirewallRuleStats(cmd.getVal(), cmd.getLen());
    }
}

void AtlasFilter::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
    if (!error) {
        ATLAS_LOGGER_DEBUG("Reading data from gateway");

        processCommand(bytesTransferred);

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

bool AtlasFilter::filter(const AtlasPacket &pkt)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string dstClientId = pkt.getDstClientId();

    /* If no rule is found for the destination client id, then packet can be processed */
    if (rules_.find(dstClientId) == rules_.end())
        return true;

    return rules_[dstClientId].filter(pkt);
}

} // namespace atlas
