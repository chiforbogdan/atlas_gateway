#ifndef __ATLAS_COMMAND_TYPE_H__
#define __ATLAS_COMMAND_TYPE_H__

namespace atlas {

enum AtlasCommandType {
    /* Register command: payload is the client identity*/
    ATLAS_CMD_REGISTER = 0,

    /* Keepalive command: payload is a 2 byte token*/
    ATLAS_CMD_KEEPALIVE,

    /* Identity command: payload is the client identity */
    ATLAS_CMD_IDENTITY,

    /* Get all publish-subscribe firewall rules: no payload */
    ATLAS_CMD_GET_ALL_PUB_SUB_FIREWALL_RULES,
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_TYPE_H__ */
