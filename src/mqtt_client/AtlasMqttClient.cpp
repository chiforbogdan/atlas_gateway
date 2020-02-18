#include "AtlasMqttClient.h"
#include "AtlasMqttException.h"
#include "../logger/AtlasLogger.h"

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

void AtlasMqttClient::connect(const std::string &address, const std::string &clientID)
{
    ATLAS_LOGGER_INFO("Initializing client (" + clientID + ") connection to server (" + address + ")");

    //creating the async_client
    try {
        if (client_ == nullptr) {
            client_ = new mqtt::async_client(address, clientID);
            client_->set_callback(cb_);
        }
    } catch(const std::exception& e) {
        ATLAS_LOGGER_ERROR(std::string(e.what()));
    }        

    //connecting to remote server
    try {
        if (!client_->is_connected()) {
            connTok_ = client_->connect(connOps_, nullptr, connectActList_);
            ATLAS_LOGGER_INFO("Connection from " + clientID + " to " + address + " will be establshed.");
        }  else {
            ATLAS_LOGGER_INFO("Connection from " + clientID + " to " + address + " is already establshed.");
        }
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected())
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
        
        throw AtlasMqttException(std::string(e));
    }
}

void AtlasMqttClient::publishMessage(const std::string &topic, const std::string &message, int QoS = 2)
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

bool AtlasMqttClient::tryPublishMessage(const std::string topic, const std::string message, const int QoS)
{
    try
    {   
        if (connTok_ == nullptr)
        {
            ATLAS_LOGGER_ERROR("No existing connection for client [" + client_->get_client_id() + "]. PUBLISH aborted.");
            return false;
        }
        else
        {
            if (!connTok_->try_wait())  //non-blocking task --> exit with false and wait for a re-call of publish
            {
                ATLAS_LOGGER_ERROR("Previous CONNECT action has not yet finished for client [" + client_->get_client_id() + "]. PUBLISH should be delayed.");
                return false;
            }
            else
            {
                mqtt::message_ptr pubMsg = mqtt::make_message(topic, message);
                pubMsg->set_qos(QoS);                
                pubTok_ = client_->publish(pubMsg, nullptr, deliveryActList_);  
                ATLAS_LOGGER_INFO("Message [" + std::to_string(pubTok_->get_message_id()) + "] will be sent with QoS " + 
                            std::to_string(QoS) + " by client [" + client_->get_client_id() + "]."); 
            }
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

    return true;
}


void AtlasMqttClient::subscribeTopic(const std::string topic, const int QoS)
{
    try
    {
        if (client_->is_connected())   
        {     
            ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] subscribing to topic [" + topic + "] with QoS " + std::to_string(QoS));
            subTok_ = client_->subscribe(topic, QoS, nullptr, subscribeActList_);
        }
        else
        {
            ATLAS_LOGGER_INFO("No active connection. SUBSCRIBE aborted.");            
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

bool AtlasMqttClient::trySubscribeTopic(const std::string topic, const int QoS)
{
    try
    {
        if (connTok_ == nullptr)
        {
            std::cout << "No existing connection. SUBSCRIBE aborted." << std::endl;
            return false;
        }
        else
        {
            if (!connTok_->try_wait())
            {
                std::cout << "Connection not yet finished. SUBSCRIBE should be delayed." << std::endl;
                return false;
            }
            else
            {
                ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] subscribing to topic [" + topic + "] with QoS " + std::to_string(QoS));
                subTok_ = client_->subscribe(topic, QoS, nullptr, subscribeActList_);
            }
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

    return true;
}


void AtlasMqttClient::disconnect()
{
    try
    {
        if (client_->is_connected())
        {
            if ((pubTok_ != nullptr) && (!pubTok_->is_complete()))
                pubTok_->wait();
                       
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
            ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] has successfully disconnected from " + client_->get_server_uri());
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

bool AtlasMqttClient::tryDisconnect()
{
    try
    {
        if (connTok_ == nullptr)
        {
            ATLAS_LOGGER_INFO("No existing connection. DISCONNECT aborted.");
            return false;
        }
        else
        {
            if (!connTok_->try_wait())
            {
                ATLAS_LOGGER_INFO("Connection is not yet finished. DISCONNECT should be delayed.");
                return false;
            }
            else
            {
                if (pubTok_ == nullptr) 
                {
                    discTok_ = client_->disconnect(nullptr, disconnectActList_);
                    ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] has successfully disconnected from " + client_->get_server_uri());
                }
                else
                {
                    if (pubTok_->try_wait())
                    {
                        discTok_ = client_->disconnect(nullptr, disconnectActList_);
                        ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] has successfully disconnected from " + client_->get_server_uri());
                    }
                    else
                    {
                        ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] has unfinished PUBLISH tasks. DISCONNECT aborted.");
                        return false;
                    }
                }
            }
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

    return true;
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
