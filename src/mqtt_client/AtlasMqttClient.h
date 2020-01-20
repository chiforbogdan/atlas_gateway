#ifndef __ATLAS_MQTT_CLIENT_H__
#define __ATLAS_MQTT_CLIENT_H__

#include <stdlib.h>
#include <string>

using namespace std;

namespace atlas
{
class AtlasMqttClient
{
public:
	static AtlasMqttClient& getInstance();

	void publishMessage(const string address, const string clientID, const string topic, 
                    const string message, int QoS);

	AtlasMqttClient(const AtlasMqttClient &) = delete;
	AtlasMqttClient & operator = (const AtlasMqttClient &) = delete;
private:	
	AtlasMqttClient() {}
};
} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_H__*/
