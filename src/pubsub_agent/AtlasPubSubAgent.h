#ifndef __ATLAS_PUBSUB_AGENT_H__
#define __ATLAS_PUBSUB_AGENT_H__

#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#define ATLAS_PUB_SUB_AGENT_BUF_LEN (2048)

namespace atlas {

class AtlasPubSubAgent
{

public:
    /**
    * @brief Ctor for publish-subscribe agent
    * @return none
    */
    AtlasPubSubAgent();

    /**
    * @brief Dtor for publish-subscribe agent
    * @return none
    */
    ~AtlasPubSubAgent();

    /**
    * @brief Start publish-subscribe agent
    * @return none
    */
    void start();

private:
    /**
    * @brief Write data to publish-subscribe agent
    * @param[in] buf Data buffer
    * @param[in] bufLen Data buffer length
    * @return none
    */
    void write(const uint8_t *buf, size_t bufLen);
    
    /**
    * @brief Get all firewall rules for publish-subscribe agent
    * @return none
    */
    void getAllFirewallRules();

    /**
    * @brief Process command received from publish-subsribe agent
    * @param[in] cmdLen Command length
    * @return none
    */
    void processCommand(size_t cmdLen);
    
    /**
    * @brief Accept connections from publish-subscribe agent
    * @param[in] error Connection error
    * @return none
    */
    void handleAccept(const boost::system::error_code& error);
    
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

    /* Publish-subscribe agent server */
    boost::asio::local::stream_protocol::acceptor acceptor_;
    
    /* Publish-subscribe agent accepting socket */
    boost::asio::local::stream_protocol::socket *acceptingSocket_;
    
    /* Publish-subscribe agent connecting socket */
    boost::asio::local::stream_protocol::socket *connectedSocket_;

    /* Read data buffer */
    uint8_t data_[ATLAS_PUB_SUB_AGENT_BUF_LEN];
};

} // namespace atlas

#endif /* __ATLAS_PUBSUB_AGENT_H__ */
