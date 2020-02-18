#include "AtlasMqttClient_callback.h"
#include "AtlasLogger.h"

namespace atlas
{

void AtlasMqttClient_callback::connection_lost(const std::string& cause)
{
    if (cause.empty())
        ATLAS_LOGGER_ERROR("Connection lost");
    else
        ATLAS_LOGGER_ERROR("Connection lost --> cause: " + cause); 
}

void AtlasMqttClient_callback::message_arrived(mqtt::const_message_ptr msg)
{
    ATLAS_LOGGER_INFO("Message arrived on topic [" + msg->get_topic() + "] with payload: \"" + msg->to_string() + "\"");
}

} //namespace atlas
