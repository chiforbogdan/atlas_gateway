#include "AtlasMqttClient.h"
#include "AtlasMqttException.h"
#include "AtlasLogger.h"

namespace atlas
{
AtlasMqttClient::AtlasMqttClient()
{
    client_ = nullptr;
    connTok_ = nullptr;
    discTok_ = nullptr;
    pubTok_ = nullptr;
    subTok_ = nullptr;
}

AtlasMqttClient& AtlasMqttClient::getInstance()
{
    static AtlasMqttClient instance;
    return instance;
}

bool AtlasMqttClient::connect(const std::string address, const std::string clientID)
{
    bool returnResult;
    ATLAS_LOGGER_INFO("Initializing client (" + clientID + ") connection to server (" + address + ")");

    try     //creating the async_client
    {
        if (client_ == nullptr)
        {
            client_ = new mqtt::async_client(address, clientID);
            client_->set_callback(cb_);
        }
    }
    catch(const std::exception& e)
    {
        ATLAS_LOGGER_ERROR(std::string(e.what()));
    }        

    try     //connecting to remote server
    {
        if (!client_->is_connected())
        {
            connTok_ = client_->connect(connOps_, nullptr, connectActList_);
            ATLAS_LOGGER_INFO("Connection from " + clientID + " to " + address + " will be establshed.");
            returnResult = true;
        }  
        else
        {
            ATLAS_LOGGER_INFO("Connection from " + clientID + " to " + address + " is already establshed.");
            returnResult = false;
        }
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected())
        {
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
        }
        throw AtlasMqttException(std::string(e));
    }

    return returnResult;
}

void AtlasMqttClient::publishMessage(const std::string topic, const std::string message, const int QoS)
{
    try
    {        
        if (!client_->is_connected())
            connTok_->wait();   //blocking task --> publish cannot be done without first establishing a connection

        mqtt::message_ptr pubMsg = mqtt::make_message(topic, message);
        pubMsg->set_qos(QoS);        
        pubTok_ = client_->publish(pubMsg, nullptr, deliveryActList_);
        ATLAS_LOGGER_INFO("Message [" + std::to_string(pubTok_->get_message_id()) + "] will be sent with QoS " + 
                            std::to_string(QoS) + " by client [" + client_->get_client_id() + "].");        
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected())
        {
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
        }
        throw AtlasMqttException(std::string(e));
    }    
}

void AtlasMqttClient::subscribeTopic(const std::string topic, const int QoS)
{
    try
    {
        if (client_->is_connected())   
        {     
            ATLAS_LOGGER_INFO("Client subscribing to topic [" + topic + "] with QoS " + std::to_string(QoS));
            subTok_ = client_->subscribe(topic, QoS, nullptr, subscribeActList_);
        }
        else
        {
            ATLAS_LOGGER_INFO("No active connection. Subscribe not executed.");            
        }
        
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected())
        {
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
        }
        throw AtlasMqttException(std::string(e));
    } 
}

void AtlasMqttClient::disconnect()
{
    try
    {
        if (client_->is_connected)
        {
            if ((pubTok_ != nullptr) && (!pubTok_->is_complete()))
                pubTok_->wait();
                       
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
            ATLAS_LOGGER_INFO("Client " + client_->get_client_id() + " has successfully disconnected from " + client_->get_server_uri());
        }
    }
    catch(const mqtt::exception& e)
    {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    }
    catch(const char* e)
    {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected())
        {
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
        }
        throw AtlasMqttException(std::string(e));
    }  
}

AtlasMqttClient::~AtlasMqttClient()
{
    this->disconnect();
    discTok_->wait();
    delete client_;
    client_ = nullptr;
    connTok_ = nullptr;
    discTok_ = nullptr;
    pubTok_ = nullptr;
    subTok_ = nullptr;
}

} //namespace atlas
