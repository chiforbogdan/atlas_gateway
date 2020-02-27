#ifndef __ATLAS_MQTT_CLIENT_CALLBACK_H__
#define __ATLAS_MQTT_CLIENT_CALLBACK_H__

#include <mqtt/async_client.h>
#include <stdlib.h>
#include <string>
#include <boost/bind.hpp>
#include "../alarm/AtlasAlarm.h"

namespace atlas
{
    
class AtlasMqttClient_callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener
{

public:
    /**
     * @brief Explicit Ctor for the callback
     * @param[in] Asynchronous MQTT client object used for connecting to a Atlas Cloud module
     * @param[in] Connection Options related to the Asynchronous client
     * @return none
    */
    AtlasMqttClient_callback(mqtt::async_client& client, mqtt::connect_options& connOpts);

private:
    /**
     * @brief Callback called when a connection is lost
     * @param[in] Reason for loosing the connection
     * @return none
    */
    void connection_lost(const std::string& cause) override;
    
    /**
     * @brief Callback called when a message is received on a subscribed topic
     * @param[in] Received message
     * @return none
    */
    void message_arrived(mqtt::const_message_ptr msg) override;

    /**
     * @brief Connection to a Atlas Cloud module fails
     * @param[in] Token of the failed connection
     * @return none
    */
    void on_failure(const mqtt::token&) override;

    /**
     * @brief Connection to a Atlas Cloud module is a success
     * @param[in] Token of the succeded connection
     * @return none
    */
    void on_success(const mqtt::token&) override {}

    /**
     * @brief Reconnects to the most recent known Atlas Cloud module
     * @return none
    */
    void reconnect();
	
    /**
     * @brief Callback for reconnect alarm
     * @return none
     */
    void alarmCallback();
    
    /* Reference to MQTT client */
    mqtt::async_client& client_;

    /* Reference to MQTT connection options */
    mqtt::connect_options& connOpts_;

    /* MQTT reconnect alarm */
    AtlasAlarm alarm_;
};

} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_CALLBACK_H__*/
