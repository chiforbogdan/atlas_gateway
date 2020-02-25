#include "AtlasMqttClient_callback.h"
#include "../logger/AtlasLogger.h"
#include "AtlasMqttException.h"

namespace atlas
{

namespace {
    const int ATLAS_MQTT_RECONNECT_INTERVAL_MS = 5000;
} // anonymous namespace

AtlasMqttClient_callback::AtlasMqttClient_callback(mqtt::async_client& client, mqtt::connect_options& connOpts) : client_(client), connOpts_(connOpts),
                                                                                                                  alarm_(ATLAS_MQTT_RECONNECT_INTERVAL_MS, true,
                                                                                                                         boost::bind(&AtlasMqttClient_callback::alarmCallback, this)) {}

void AtlasMqttClient_callback::connection_lost(const std::string& cause)
{
    if (cause.empty())
        ATLAS_LOGGER_INFO("AtlasMqttClient_Callback (connection_lost): Connection lost. Trying to reconnect...");
    else
        ATLAS_LOGGER_INFO("Callback (connection_lost): Connection lost --> cause: " + cause);

    alarm_.start();
}

void AtlasMqttClient_callback::message_arrived(mqtt::const_message_ptr msg)
{
    ATLAS_LOGGER_INFO("AtlasMqttClient_Callback (message_arrived): Message arrived on topic [" + msg->get_topic() + "] with payload: \"" + msg->to_string() + "\"");
}

void AtlasMqttClient_callback::alarmCallback()
{
    ATLAS_LOGGER_INFO("AtlasMqttClient_Callback reconnect alarm");

    reconnect();
}

void AtlasMqttClient_callback::reconnect()
{
    try {
        client_.connect(connOpts_, nullptr, *this);
    } catch (const mqtt::exception& e) {
        ATLAS_LOGGER_DEBUG("AtlasMqttClient_Callback (reconnect): Error on reconnect --> what(): " + std::string(e.what()));
        throw AtlasMqttException(e.what());
    }
}

void AtlasMqttClient_callback::on_failure(const mqtt::token& tok)
{
    ATLAS_LOGGER_DEBUG("AtlasMqttClient_Callback (on_failure): Connection attempt failure. Trying to reconnect...");

    alarm_.start();
}

} //namespace atlas
