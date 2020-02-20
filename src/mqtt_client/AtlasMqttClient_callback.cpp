#include "AtlasMqttClient_callback.h"
#include "../logger/AtlasLogger.h"
#include "AtlasMqttException.h"

namespace atlas
{

void AtlasMqttClient_callback::connection_lost(const std::string& cause)
{
    if (cause.empty())
        ATLAS_LOGGER_INFO("Callback (connection_lost): Connection lost");
    else
        ATLAS_LOGGER_INFO("Callback (connection_lost): Connection lost --> cause: " + cause);
    
    ATLAS_LOGGER_INFO("Callback (connection_lost): Trying to reconnect...");
    retryCount = 0;
    reconnect();
}

void AtlasMqttClient_callback::message_arrived(mqtt::const_message_ptr msg)
{
    ATLAS_LOGGER_INFO("Callback (message_arrived): Message arrived on topic [" + msg->get_topic() + "] with payload: \"" + msg->to_string() + "\"");
}


void AtlasMqttClient_callback::reconnect()
{
    sleep(2);
    try {
        client_.connect(connOpts_, nullptr, *this);
    }
    catch (const mqtt::exception& e) {
        ATLAS_LOGGER_DEBUG("Callback (reconnect): Error on reconnect --> what(): " + std::string(e.what()));
        throw AtlasMqttException(e.what());
    }
}


void AtlasMqttClient_callback::on_failure(const mqtt::token& tok)
{
    ATLAS_LOGGER_DEBUG("Callback (on_failure): Connection attempt failure");
    if (retryCount < maxNoOfRetry)
        {
            retryCount++;
            ATLAS_LOGGER_INFO("Callback (on_failure): Trying to reconnect --> retry number " + std::to_string(AtlasMqttClient_callback::retryCount) + " of " + std::to_string(maxNoOfRetry));
            reconnect();          
        }
        else
        {
            ATLAS_LOGGER_ERROR("Callback (on_failure): Maximum number of reconnect retries has been reached.");
            throw AtlasMqttException("Maximum number of reconnect retries has been reached.");
        }
}

} //namespace atlas
