#ifndef __ATLAS_HTTP_SERVER_H__
#define __ATLAS_HTTP_SERVER_H__

#include <string>
#include <nghttp2/asio_http2_server.h>

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

namespace atlas {

class AtlasHttpServer {

public:
    /**
     * @brief Get HTTP server instance
     * @return HTTP server instance
     */
    static AtlasHttpServer& getInstance();

    /**
     * @brief Start HTTP2 server
     * @return true if server is started, false otherwise
     */
    bool start(const std::string &certFile, const std::string &privKeyFile, int port);

    /**
     * @brief Stop HTTP server
     * @return none
     */
    void stop();

private:
    /**
     * @brief Default ctor
     * @return none
     */
    AtlasHttpServer();
    
    /**
     * @brief Copy ctor
     * @return none
     */
    AtlasHttpServer(const AtlasHttpServer&) = delete;

    /* HTTP2 server */
    http2 server_;

    /* TLS context */
    boost::asio::ssl::context tls_;
};

} // namespace atlas

#endif /* __ATLAS_HTTP_SERVER_H__ */
