#ifndef __ATLAS_MQTT_EXCEPTION_H__
#define __ATLAS_MQTT_EXCEPTION_H__

#include <exception>
#include <string>

namespace atlas
{

class AtlasMqttException: public std::exception
{
public:
	AtlasMqttException(const std::string &message);
	virtual const char* what() const throw();

private:
	std::string message_;
};

} //namespace atlas

#endif /* __ATLAS_MQTT_EXCEPTION_H__ */
