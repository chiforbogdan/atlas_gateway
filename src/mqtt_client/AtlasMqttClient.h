#ifndef __ATLAS_MQTT_CLIENT_H__
#define __ATLAS_MQTT_CLIENT_H__

#include <stdlib.h>
#include <string>
#include <mqtt/async_client.h>
#include "AtlasMqttClient_callback.h"
#include "AtlasMqttClient_actionListener.h"

namespace atlas
{
class AtlasMqttClient
{
public:
    /**
     * @brief Get MQTT client instance
     * @return MQTT client instance
    */
    static AtlasMqttClient& getInstance();

    /**
     * @brief Connect to a Atlas Cloud module with explicit parameters
     * @param[in] IP address of the Atlas Cloud module
     * @param[in] Gateway ID
     * @return none
    */
    void connect(const std::string &address, const std::string &clientID);
    
    /**
     * @brief Connect to a Atlas Cloud module with command line arguments
     * @param[in] First command line argument (IP address of the Atlas Cloud module)
     * @return none
    */
    void connect(const char *arg);


    #pragma region Blocking methods
    /**
     * @brief Subscribe to a topic on the Atlas Cloud module
     * @param[in] Topic to subscribe to
     * @param[in] Desired QoS
     * @return True on success, False otherwise
    */
    bool subscribeTopic(const std::string &topic, const int QoS);

    /**
     * @brief Disconnect from a Atlas Cloud module
     * @return none
    */
    void disconnect();
	#pragma endregion

    #pragma region Non-blocking methods
    /**
     * @brief Non-blocking Publish of a message to a specific topic
     * @param[in] Topic to publish the message to
     * @param[in] Message to be published
     * @param[in] QoS for message publishing
     * @return True on publish succes, False otherwise 
    */
    bool tryPublishMessage(const std::string &topic, const std::string &message, const int QoS);
    #pragma endregion

    AtlasMqttClient(const AtlasMqttClient &) = delete;
    AtlasMqttClient & operator = (const AtlasMqttClient &) = delete;

    /**
     * @brief Default Dtor for MQTT client
     * @return none 
    */
    ~AtlasMqttClient();

    /**
     * @brief Set ID for gateway
     * @param[in] Gateway ID
     * @return none
    */
    void setClientID(std::string id) { clientID_ = id; }
    
    /**
     * @brief Get gateway ID
     * @return Gateway ID as std::string
    */
    std::string getClientID() { return clientID_; }
private:	
    std::string cloudHost_;
    std::string clientID_;
    mqtt::async_client *client_;
    mqtt::token_ptr connTok_, discTok_;
    mqtt::delivery_token_ptr pubTok_;
    mqtt::token_ptr subTok_;
    mqtt::connect_options connOps_;
    AtlasMqttClient_callback *cb_;
    AtlasMqttClient_connectActionListener connectActList_;
    AtlasMqttClient_deliveryActionListener deliveryActList_;
    AtlasMqttClient_disconnectActionListener disconnectActList_;
    AtlasMqttClient_subscriptionActionListener subscribeActList_;

    /**
     * @brief Default ctor for MQTT client
     * @return none
    */
    AtlasMqttClient();
};
} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_H__*/
