#include <string>
#include "AtlasMqttClient_actionListener.h"
#include "AtlasLogger.h"

namespace atlas
{

//***********Class base Action Listener*******************
void AtlasMqttClient_actionListener::on_failure(const mqtt::token& tok)
{
    ATLAS_LOGGER_INFO("Listener failure for token: " + tok.get_message_id());
}

void AtlasMqttClient_actionListener::on_success(const mqtt::token& tok)
{
    ATLAS_LOGGER_INFO("Listener succes for token: " + tok.get_message_id());
}

//************Class Connecting Action Listener**************
void AtlasMqttClient_connectActionListener::on_failure(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_failure(tok);
    done_ = true;
}

void AtlasMqttClient_connectActionListener::on_success(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_success(tok);
    done_ = true;
}


//************Class Delivery Action Listener****************
void AtlasMqttClient_deliveryActionListener::on_failure(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_failure(tok);
    done_ = true;
}

void AtlasMqttClient_deliveryActionListener::on_success(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_success(tok);
    done_ = true;
}


//************Class Disconnecting Action Listener**************
void AtlasMqttClient_disconnectActionListener::on_failure(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_failure(tok);
    done_ = true;
}

void AtlasMqttClient_disconnectActionListener::on_success(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_success(tok);
    done_ = true;
}


//*************Class Subscribe Action Listener******************
void AtlasMqttClient_subscriptionActionListener::on_failure(const mqtt::token& tok)
{
    std::string tmpMsg = "Subscription failure";
    if (tok.get_message_id() != 0)
        tmpMsg = tmpMsg + " for token: [" + std::to_string(tok.get_message_id()) + "]";
    ATLAS_LOGGER_INFO(tmpMsg);
    done_ = false;
}

void AtlasMqttClient_subscriptionActionListener::on_success(const mqtt::token& tok)
{
    std::string tmpMsg = "Subscription success";
    if (tok.get_message_id() != 0)
        tmpMsg = tmpMsg + " for token: [" + std::to_string(tok.get_message_id()) + "]";
    auto tmpTopic = tok.get_topics();
    if (tmpTopic && !tmpTopic->empty())
        tmpMsg = tmpMsg + ". Token topic: \"" + (*tmpTopic)[0] + "\"";
    ATLAS_LOGGER_INFO(tmpMsg);
    done_ = true;
}

} //namespace atlas