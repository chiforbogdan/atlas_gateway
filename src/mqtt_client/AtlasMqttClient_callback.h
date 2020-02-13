#ifndef __ATLAS_MQTT_CLIENT_CALLBACK_H__
#define __ATLAS_MQTT_CLIENT_CALLBACK_H__

#include <mqtt/async_client.h>
#include <stdlib.h>
#include <string>


namespace atlas
{
    
class AtlasMqttClient_callback : public virtual mqtt::callback
{
public:
    void connection_lost(const std::string& cause) override;
    void message_arrived(mqtt::const_message_ptr msg);
};

} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_CALLBACK_H__*/