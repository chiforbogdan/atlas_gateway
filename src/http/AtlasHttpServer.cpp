#include <iostream>
#include "AtlasHttpServer.h"
#include "../logger/AtlasLogger.h"

namespace atlas {

namespace {
    const std::string INDEX = "/";
    const std::string INDEX_PAYLOAD = "ATLAS Gateway internal HTTP2 server!";
} // anonymous namespace

AtlasHttpServer& AtlasHttpServer::getInstance()
{
    static AtlasHttpServer server;

    return server;
}

AtlasHttpServer::AtlasHttpServer() : tls_(boost::asio::ssl::context::tlsv13) {}

bool AtlasHttpServer::start(const std::string &certFile, const std::string &privKeyFile, int port)
{
    boost::system::error_code ec;

    tls_.use_private_key_file(privKeyFile, boost::asio::ssl::context::pem);
    tls_.use_certificate_chain_file(certFile);

    configure_tls_context_easy(ec, tls_);

    /* Install default index handler */
    server_.handle(INDEX, [](const request &req, const response &res) {
        res.write_head(200);
        res.end(INDEX_PAYLOAD);
    });

    if (server_.listen_and_serve(ec, tls_, "localhost", std::to_string(port), true)) {
    	ATLAS_LOGGER_ERROR("Error when starting HTTP2 server (listen and server)");
        return false;
    }

    return true;
}

void AtlasHttpServer::stop()
{
    /* Stop HTTP2 server */
    server_.join();
    server_.stop();
}

} // namespace atlas
