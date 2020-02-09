#include "AtlasMqttClient.h"
#include "AtlasMqttException.h"
#include "AtlasLogger.h"

namespace atlas
{
AtlasMqttClient& AtlasMqttClient::getInstance()
{
    static AtlasMqttClient instance;
    return instance;
}

void AtlasMqttClient::connect(const std::string address, const std::string clientID)
{
    ATLAS_LOGGER_INFO("Initializing client (" + clientID + ") connection to server (" + address + ")");

    try     //creating the async_client
    {
        client_ = mqtt::async_client::ptr_t(&mqtt::async_client(address, clientID));
        client_->set_callback(cb_);
    }
    catch(const std::exception& e)
    {
        ATLAS_LOGGER_ERROR(std::string(e.what()));
    }        

    try     //connecting to remote server
    {
        connTok_ = client_->connect(connOps_, nullptr, connectActList_);
        //connTok_->wait();
        ATLAS_LOGGER_INFO("Connection from " + clientID + " to " + address + " is establshed.");        
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        connTok_ = client_->disconnect(nullptr, disconnectActList_);
        //connTok_->wait();
        throw AtlasMqttException(std::string(e));
    }
}

void AtlasMqttClient::publishMessage(const std::string topic, const std::string message, const int QoS)
{
    try
    {        
        mqtt::message_ptr pubMsg = mqtt::make_message(topic, message);
        pubMsg->set_qos(QoS);        
        pubTok_ = client_->publish(pubMsg, nullptr, deliveryActList_);
        //pubTok_->wait();
        ATLAS_LOGGER_INFO("Message sent with QoS " + std::to_string(QoS));        
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        connTok_ = client_->disconnect(nullptr, disconnectActList_);
        //connTok_->wait();
        throw AtlasMqttException(std::string(e));
    }    
}

void AtlasMqttClient::subscribeTopic(const std::string topic, const int QoS)
{
    try
    {        
        ATLAS_LOGGER_INFO("Client subscribing to topic [" + topic + "] with QoS " + std::to_string(QoS));
        subTok_ = client_->subscribe(topic, QoS, nullptr, subscribeActList_);
        //subTok_->wait();        
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        //subTok_ = client_->disconnect(nullptr, disconnectActList_);
        //subTok_->wait();
        throw AtlasMqttException(std::string(e));
    } 
}

void AtlasMqttClient::disconnect()
{
    try
    {
        auto tokens = client_->get_pending_delivery_tokens();
        if (!tokens.empty())        
            throw "Error: Not all deliveries have finished. Pending delivery tokens exist.";
        
        connTok_ = client_->disconnect(nullptr, disconnectActList_);
        //connTok_->wait();
        ATLAS_LOGGER_INFO("Client " + client_->get_client_id() + " has successfully disconnected from " + client_->get_server_uri());
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        connTok_ = client_->disconnect(nullptr, disconnectActList_);
        //connTok_->wait();
        throw AtlasMqttException(std::string(e));
    }  
}

AtlasMqttClient::~AtlasMqttClient()
{
    this->disconnect();
}

} //namespace atlas
