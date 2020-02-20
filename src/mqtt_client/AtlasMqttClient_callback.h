#ifndef __ATLAS_MQTT_CLIENT_CALLBACK_H__
#define __ATLAS_MQTT_CLIENT_CALLBACK_H__

#include <mqtt/async_client.h>
#include <stdlib.h>
#include <string>


namespace atlas
{
    
class AtlasMqttClient_callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
    void connection_lost(const std::string& cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

    void reconnect();

    void on_failure(const mqtt::token&) override;
    void on_success(const mqtt::token&) override {}
public:
    AtlasMqttClient_callback(mqtt::async_client& client, int maxNumberOfReconnects, mqtt::connect_options& connOpts) : client_(client), maxNoOfRetry(maxNumberOfReconnects), 
                                                                                                                            retryCount(0), connOpts_(connOpts) {};
private:
    int retryCount;
    int maxNoOfRetry;
    mqtt::async_client& client_;
    mqtt::connect_options& connOpts_;
};

} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_CALLBACK_H__*/