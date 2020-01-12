#include <iostream>
#include "scheduler/AtlasScheduler.h"
#include "coap/AtlasCoapServer.h"
#include "coap/AtlasCoapResource.h"
#include "coap/AtlasCoapMethod.h"
#include "coap/AtlasCoapResponse.h"
#include "logger/AtlasLogger.h"
#include "alarm/AtlasAlarm.h"
#include "coap/AtlasCoapClient.h"

void test_get(atlas::AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
{
    std::cout << "test_get called" << std::endl;

    std::cout << "status" << respStatus << std::endl;

    for (int i = 0; i < resp_payload_len; i++)
        std::cout << resp_payload[i];

    std::cout << std::endl;
}

 void test_post(atlas::AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
 {
    std::cout << "test_post called" << std::endl;

    std::cout << "status" << respStatus << std::endl;
 
    for (int i = 0; i < resp_payload_len; i++)
        std::cout << resp_payload[i];

    std::cout << std::endl;
}

 void test_put(atlas::AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
 {
    std::cout << "test_put called" << std::endl;

    std::cout << "status" << respStatus << std::endl;
 
    for (int i = 0; i < resp_payload_len; i++)
        std::cout << resp_payload[i];

    std::cout << std::endl;
}

 void test_del(atlas::AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
 {
    std::cout << "test_del called" << std::endl;

    std::cout << "status" << respStatus << std::endl;
 
    if (resp_payload && resp_payload_len)
        for (int i = 0; i < resp_payload_len; i++)
            std::cout << resp_payload[i];

    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    atlas::initLog();

    atlas::AtlasCoapServer::getInstance().start("127.0.0.1", "10099", atlas::ATLAS_COAP_SERVER_MODE_BOTH, "12345678"); 

    uint8_t reqPayload[30];
    memset(reqPayload, 'A', 30);
    atlas::AtlasCoapClient::getInstance().sendRequest("coap://127.0.0.1:10001/test/test_get", atlas::ATLAS_COAP_METHOD_GET, reqPayload,
                                                      30, 5000, test_get);


    memset(reqPayload, 'B', 30);
    atlas::AtlasCoapClient::getInstance().sendRequest("coap://127.0.0.1:10001/test/test_post", atlas::ATLAS_COAP_METHOD_POST, reqPayload,
                                                      30, 5000, test_post);


    memset(reqPayload, 'C', 30);
    atlas::AtlasCoapClient::getInstance().sendRequest("coap://127.0.0.1:10001/test/test_put", atlas::ATLAS_COAP_METHOD_PUT, reqPayload,
                                                      30, 5000, test_put);

    memset(reqPayload, 'D', 30);
    atlas::AtlasCoapClient::getInstance().sendRequest("coap://127.0.0.1:10001/test/test_del", atlas::ATLAS_COAP_METHOD_DELETE, reqPayload,
                                                      30, 5000, test_del);
    
    atlas::AtlasCoapClient::getInstance().sendRequest("coap://127.0.0.1:10003/test/test_del1", atlas::ATLAS_COAP_METHOD_DELETE, reqPayload,
                                                      30, 5000, test_del);
    
    ATLAS_LOGGER_DEBUG("Starting Atlas gateway...");

    atlas::AtlasScheduler::getInstance().run();

    return 0;
};
