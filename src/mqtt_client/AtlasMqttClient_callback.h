#ifndef __ATLAS_MQTT_CLIENT_CALLBACK_H__
#define __ATLAS_MQTT_CLIENT_CALLBACK_H__

#include <mqtt/async_client.h>
#include <stdlib.h>
#include <string>

using namespace std;

namespace atlas
{
class AtlasMqttClient_callback : public virtual mqtt::callback
{
public:
    void connection_lost(const string& cause) override;
    void delivery_complete(mqtt::delivery_token_ptr tok) override;
};
} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_CALLBACK_H__*/