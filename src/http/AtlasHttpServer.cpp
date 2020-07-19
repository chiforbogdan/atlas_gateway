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

bool AtlasHttpServer::start()
{
    boost::system::error_code ec;

    /* Install default index handler */
    server_.handle(INDEX, [](const request &req, const response &res) {
        res.write_head(200);
        res.end(INDEX_PAYLOAD);
    });

    if (server_.listen_and_serve(ec, "localhost", "3000", true)) {
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
