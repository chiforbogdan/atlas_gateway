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
    cb_ = nullptr;
    clientID_ = "testGW";
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
        }
    } catch(const std::exception& e) {
        ATLAS_LOGGER_ERROR(std::string(e.what()));
    }        

    //connecting to remote server
    try {
        if (!client_->is_connected()) {
            connOps_.set_clean_session(true);
            cb_ = new AtlasMqttClient_callback(*client_, connOps_);
            client_->set_callback(*cb_);
            connTok_ = client_->connect(connOps_, nullptr, *cb_);
            ATLAS_LOGGER_INFO("Connection from " + clientID + " to " + address + " will be establshed.");
        }  else {
            ATLAS_LOGGER_INFO("Connection from " + clientID + " to " + address + " is already establshed.");
        }
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_connect: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected())
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
        
        throw AtlasMqttException(std::string(e));
    }
}


bool AtlasMqttClient::tryPublishMessage(const std::string &topic, const std::string &message, const int QoS)
{
    try {   
        if (connTok_ == nullptr) {
            ATLAS_LOGGER_ERROR("No existing connection for client [" + client_->get_client_id() + "]. PUBLISH aborted.");
            return false;
        } else {
            if (!client_->is_connected()) { //non-blocking task --> exit with false and wait for a re-call of publish            
                ATLAS_LOGGER_ERROR("Previous CONNECT action has not yet finished for client [" + client_->get_client_id() + "]. PUBLISH should be delayed.");
                return false;
            } else {
                mqtt::message_ptr pubMsg = mqtt::make_message(topic, message);
                pubMsg->set_qos(QoS);                
                pubTok_ = client_->publish(pubMsg, nullptr, deliveryActList_);  
                ATLAS_LOGGER_INFO("Message [" + std::to_string(pubTok_->get_message_id()) + "] will be sent with QoS " + 
                            std::to_string(QoS) + " by client [" + client_->get_client_id() + "]."); 
            }
        }
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_tryPublish: " + std::string(e.what()));
        throw AtlasMqttException(std::string(e.what()));
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected())        
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
        
        throw AtlasMqttException(std::string(e));
    }

    return true;
}


bool AtlasMqttClient::subscribeTopic(const std::string &topic, const int QoS)
{
    try {
        if (client_->is_connected()) {     
            ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] subscribing to topic [" + topic + "] with QoS " + std::to_string(QoS));
            subTok_ = client_->subscribe(topic, QoS, nullptr, subscribeActList_);
        } else {
            ATLAS_LOGGER_ERROR("No active connection. SUBSCRIBE aborted.");    
            return false;        
        }
        
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_subscribe: " + std::string(e.what()));

        throw AtlasMqttException(std::string(e.what()));
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected()) 
            discTok_ = client_->disconnect(nullptr, disconnectActList_);        

        throw AtlasMqttException(std::string(e));
    } 
    return true;
}

void AtlasMqttClient::disconnect()
{
    try {
        if (client_->is_connected()) {
            if ((pubTok_ != nullptr) && (!pubTok_->is_complete()))
                pubTok_->wait();
                       
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
            ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] has successfully disconnected from " + client_->get_server_uri());
        }
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_disconnect: " + std::string(e.what()));
        
        throw AtlasMqttException(std::string(e.what()));
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
        if (client_->is_connected()) 
            discTok_ = client_->disconnect(nullptr, disconnectActList_);

        throw AtlasMqttException(std::string(e));
    }  
}

void AtlasMqttClient::addConnectionCb(IAtlasMqttState *connCb)
{
    /* If callback is invalid or client callback is not created yet */
    if (!connCb || !cb_)
        return;

    cb_->addConnectionCb(connCb);
}

void AtlasMqttClient::removeConnectionCb(IAtlasMqttState *connCb)
{
    /* If callback is invalid or client callback is not created yet */
    if (!connCb || !cb_)
        return;
    
    cb_->removeConnectionCb(connCb);
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

    delete cb_;
    cb_ = nullptr;
}

} //namespace atlas
