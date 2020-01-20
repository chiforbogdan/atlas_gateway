#include "AtlasMqttException.h"

namespace atlas
{

AtlasMqttException::AtlasMqttException(const std::string &message) : message_(message) {}

const char* AtlasMqttException::what() const throw()
{
	return message_.c_str();
}

} //namespace atlas