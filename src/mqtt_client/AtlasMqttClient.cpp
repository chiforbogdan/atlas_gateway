#include "AtlasMqttClient.h"
#include "AtlasMqttClient_callback.h"
#include "AtlasMqttClient_actionListener.h"
#include "AtlasMqttException.h"
#include "AtlasLogger.h"
#include <mqtt/async_client.h>

namespace atlas
{
AtlasMqttClient& AtlasMqttClient::getInstance()
{
    static AtlasMqttClient instance;
    return instance;
}

void AtlasMqttClient::publishMessage(const string address, const string clientID, const string topic, 
                    const string message, int QoS)
{
    ATLAS_LOGGER_DEBUG("Initializing client (" + clientID + ") connection to server (" + address + ")");

    mqtt::async_client client(address, clientID);
    AtlasMqttClient_callback cb;
    client.set_callback(cb);
    mqtt::connect_options connOps;
    
    try
    {
        mqtt::token_ptr connTok = client.connect(connOps);
        connTok->wait();
        ATLAS_LOGGER_DEBUG("Connection from " + clientID + " to " + address + " is establshed.");

        mqtt::message_ptr pubMsg = mqtt::make_message(topic, message);
        pubMsg->set_qos(QoS);        
        mqtt::delivery_token_ptr pubTok;
        AtlasMqttClient_actionListener listener;
        pubTok = client.publish(pubMsg, nullptr, listener);
        pubTok->wait();
        ATLAS_LOGGER_DEBUG("Message sent with QoS " + QoS);

        auto tokens = client.get_pending_delivery_tokens();
        if (!tokens.empty())        
            throw "Error: Not all deliveries have finished. Pending delivery tokens exist.";        
        
        connTok = client.disconnect();
        connTok->wait();
        ATLAS_LOGGER_DEBUG("Client " + clientID + " has successfully disconnected from " + address);
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + string(e.what()));
        throw AtlasMqttException(string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(string(e));
        mqtt::token_ptr tmpConnTok = client.disconnect();
        tmpConnTok->wait();
        throw AtlasMqttException(string(e));
    }    
}

} //namespace atlas
