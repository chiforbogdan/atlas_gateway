#ifndef __ATLAS_FILTER_H__
#define __ATLAS_FILTER_H__

#include <boost/asio.hpp>

#define ATLAS_PUB_SUB_AGENT_BUF_LEN (2048)

namespace atlas {

class AtlasFilter
{

public:
    static AtlasFilter& getInstance();
    void init();

    AtlasFilter(const AtlasFilter &) = delete;
    AtlasFilter &operator=(const AtlasFilter &) = delete;
private:
    AtlasFilter();

    void gatewayConnect();

    void gatewayLoop();
    
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

};

} // namespace atlas

#endif /* __ATLAS_FILTER_H__ */
