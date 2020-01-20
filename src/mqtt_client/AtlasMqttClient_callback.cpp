#include "AtlasMqttClient_callback.h"
#include "AtlasLogger.h"

namespace atlas
{
void AtlasMqttClient_callback::connection_lost(const string& cause)
{
    if (cause.empty())
    {
        ATLAS_LOGGER_ERROR("Connection lost");
    }
    else
    {
        ATLAS_LOGGER_ERROR("Connection lost --> cause: " + cause);
    }    
}

void AtlasMqttClient_callback::delivery_complete(mqtt::delivery_token_ptr tok)
{
    ATLAS_LOGGER_DEBUG("Delivery complete for token: " + (tok ? tok->get_message_id() : -1));
}

} //namespace Atlas