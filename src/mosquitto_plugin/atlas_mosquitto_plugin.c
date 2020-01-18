#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include <fnmatch.h>
#include <time.h>
#include "atlas_mosquitto_logger.h"

#define ATLAS_MOSQUITTO_PLUGIN_START_MSG "Atlas gateway mosquitto plug-in starting..."

int mosquitto_auth_plugin_version(void)
{
    return MOSQ_AUTH_PLUGIN_VERSION;
}

int mosquitto_auth_plugin_init(void **user_data, struct mosquitto_opt *opts, int opt_count)
{
    atlas_mosq_log_init();

    ATLAS_MOSQ_LOGGER_DEBUG(ATLAS_MOSQUITTO_PLUGIN_START_MSG);
    
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_plugin_cleanup(void *userdata, struct mosquitto_opt *auth_opts, int auth_opt_count)
{
    atlas_mosq_log_close();
    
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_init(void *userdata, struct mosquitto_opt *auth_opts, int auth_opt_count, bool reload)
{
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_cleanup(void *userdata, struct mosquitto_opt *auth_opts, int auth_opt_count, bool reload)
{
    return MOSQ_ERR_SUCCESS;
}


int mosquitto_auth_unpwd_check(void *user_data, struct mosquitto *client, const char *username, const char *password)
{
    return 0;
}

int mosquitto_auth_acl_check(void *user_data, int access, struct mosquitto *client, const struct mosquitto_acl_msg *msg)
{
    return 0;
}
