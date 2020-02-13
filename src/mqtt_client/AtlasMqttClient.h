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
	bool connect(const std::string address, const std::string clientID);

	void publishMessage(const std::string topic, const std::string message, const int QoS);

	void subscribeTopic(const std::string topic, const int QoS);

	void disconnect();

	AtlasMqttClient(const AtlasMqttClient &) = delete;
	AtlasMqttClient & operator = (const AtlasMqttClient &) = delete;

	~AtlasMqttClient();
private:	
	mqtt::async_client *client_;
	mqtt::token_ptr connTok_, discTok_;
	mqtt::delivery_token_ptr pubTok_;
	mqtt::token_ptr subTok_;
	mqtt::connect_options connOps_;
	atlas::AtlasMqttClient_callback cb_;
	atlas::AtlasMqttClient_connectActionListener connectActList_;
	atlas::AtlasMqttClient_deliveryActionListener deliveryActList_;
	atlas::AtlasMqttClient_disconnectActionListener disconnectActList_;
	atlas::AtlasMqttClient_subscriptionActionListener subscribeActList_;

	AtlasMqttClient();
};
} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_H__*/
