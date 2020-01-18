#ifndef __ATLAS_MOSQUITTO_LOGGER_H__
#define __ATLAS_MOSQUITTO_LOGGER_H__

#include <syslog.h>
#include <mosquitto.h>

#define ATLAS_MOSQ_LOGGER_DEBUG(...) syslog(LOG_DEBUG, __VA_ARGS__)
#define ATLAS_MOSQ_LOGGER_INFO(...) syslog(LOG_INFO, __VA_ARGS__)
#define ATLAS_MOSQ_LOGGER_ERROR(...) syslog(LOG_ERR, __VA_ARGS__)

/**
* @brief Init logger
* @return none
*/
void atlas_mosq_log_init();

/**
* @brief Close logger
* @return none
*/
void atlas_mosq_log_close();

#endif /* __ATLAS_MOSQUITTO_LOG_H__ */
