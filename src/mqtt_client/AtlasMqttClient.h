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
    static AtlasMqttClient& getInstance();
    void connect(const std::string &address, const std::string &clientID, const int maxNoOfReconnects);
    void connect(const char *arg, const int maxNoOfReconnects);
    void reconnect();

    //Blocking methods
    bool publishMessage(const std::string &topic, const std::string &message, int QoS);	
    void subscribeTopic(const std::string &topic, const int QoS);
    void disconnect();
	
    //Non-blocking methods
    bool tryPublishMessage(const std::string &topic, const std::string &message, const int QoS);
    bool trySubscribeTopic(const std::string &topic, const int QoS);
    bool tryDisconnect();

    AtlasMqttClient(const AtlasMqttClient &) = delete;
    AtlasMqttClient & operator = (const AtlasMqttClient &) = delete;

    ~AtlasMqttClient();

    void setClientID(std::string id) { clientID_ = id; }
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

    AtlasMqttClient();
};
} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_H__*/
