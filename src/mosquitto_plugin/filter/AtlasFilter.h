#ifndef __ATLAS_FILTER_H__
#define __ATLAS_FILTER_H__

#include <boost/asio.hpp>
#include <mutex>
#include <unordered_map>
#include "AtlasPacket.h"
#include "AtlasPacketPolicer.h"

#define ATLAS_PUB_SUB_AGENT_BUF_LEN (2048)

namespace atlas {

class AtlasFilter
{

public:
    /**
    * @brief Get filter instance
    * @return none
    */
    static AtlasFilter& getInstance();

    /**
    * brief Init filter instance
    * @return none
    */
    void init();

    /**
    * @brief Filter publish-subscribe packet
    * @param[in] packet Publish-subscribe packet
    * @return True if packet should be processed, False if packet should be dropped
    */
    bool filter(const AtlasPacket &pkt);

    AtlasFilter(const AtlasFilter &) = delete;
    AtlasFilter &operator=(const AtlasFilter &) = delete;

private:
    /**
    * @brief Default ctor for filter
    * @return none
    */
    AtlasFilter();

    /**
    * @brief Connect to gateway
    * @return none
    */
    void gatewayConnect();

    /**
    * @brief Main loop for gateway connection
    * @return none
    */
    void gatewayLoop();

    /**
    * @brief Internal timer handler
    * @param[in] ec Error code
    * @return none
    */
    void timerHandler(const boost::system::error_code& ec);

    /**
    * @brief Write firewall rule statistics to gateway
    * @param[in] clientId Client id associated with the firewall rule
    * @param[in] droppedPkts Number of dropped packets
    * @param[in] passedPkts Number of accepted packets
    */
    void writeFirewallRuleStats(const std::string &clientId, uint32_t droppedPkts, uint32_t passedPkts);

    /**
    * @brief Get statistics for firewall rule
    * @param[in] cmdBuf Command buffer
    * @param[in] cmdLen Command length
    * @return none
    */
    void getFirewallRuleStats(const uint8_t *cmdBuf, uint16_t cmdLen);

    /**
    * @brief Remove firewall rule
    * @param[in] cmdBuf Command buffer
    * @param[in] cmdLen Command length
    * @return none
    */
    void removeFirewallRule(const uint8_t *cmdBuf, uint16_t cmdLen);

    /**
    * @brief Install firewall rule
    * @param[in] cmdBuf Command buffer
    * @param[in] cmdLen Command length
    * @return none
    */
    void installFirewallRule(const uint8_t *cmdBuf, uint16_t cmdLen);

    /**
    * @brief Process command from gateway
    * @param[in] cmdLen Command length
    * @return none
    */ 
    void processCommand(size_t cmdLen);
    
    /**
    * @brief Read data from publish-subscribe agent
    * @param[in] error Read error
    * @param[in] bytesTransferred Number of bytes transferred
    * @return none
    */
    void handleRead(const boost::system::error_code& error, size_t bytesTransferred);
 
    /**
    * @brief Write data to publish-subscribe agent
    * @param[in] error Write error
    * @return none
    */
    void handleWrite(const boost::system::error_code& error);
 
    /* Boost io_service */
    boost::asio::io_service ioService_;

    /* Boost io_service work (main loop) */
    boost::asio::io_service::work work_;

    /* Gateway socket */
    boost::asio::local::stream_protocol::socket *socket_;

    /* Read data buffer */
    uint8_t data_[ATLAS_PUB_SUB_AGENT_BUF_LEN];

    /* Filter rules lock */
    std::mutex mutex_;

    /* Filter rules */
    std::unordered_map<std::string, AtlasPacketPolicer> rules_;

    /* Fixed window rate limiting timer object */
    boost::asio::deadline_timer timer_;
};

} // namespace atlas

#endif /* __ATLAS_FILTER_H__ */
