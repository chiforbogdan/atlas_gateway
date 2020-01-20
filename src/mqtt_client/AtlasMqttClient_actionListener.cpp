#include "AtlasMqttClient_actionListener.h"
#include "AtlasLogger.h"

namespace atlas
{
void AtlasMqttClient_actionListener::on_failure(const mqtt::token& tok)
{
    ATLAS_LOGGER_ERROR("Listener failure for token: " + tok.get_message_id());
}

void AtlasMqttClient_actionListener::on_success(const mqtt::token& tok)
{
    ATLAS_LOGGER_ERROR("Listener succes for token: " + tok.get_message_id());
}

void AtlasMqttClient_deliveryActionListener::on_failure(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_failure(tok);
    done_ = true;
}

void AtlasMqttClient_deliveryActionListener::on_success(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_success(tok);
    done_ = true;
}

} //namespace atlas