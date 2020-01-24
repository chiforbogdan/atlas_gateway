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

    /* Install publish-subscribe firewall rule: payload is ATLAS_CMD_PUB_SUB_CLIENT_ID,
    ATLAS_CMD_PUB_SUB_MAX_QOS, ATLAS_CMD_PUB_SUB_PPS, ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN,*/
    ATLAS_CMD_PUB_SUB_INSTALL_FIREWALL_RULE,

    /* Remove publish-subscribe firewall rule: payload is ATLAS_CMD_PUB_SUB_CLIENT_ID */
    ATLAS_CMD_PUB_SUB_REMOVE_FIREWALL_RULE,

    /* Get statistics for publish-subscribe firewall rule: payload is ATLAS_CMD_PUB_SUB_CLIENT_ID */
    ATLAS_CMD_PUB_SUB_GET_STAT_FIREWALL_RULE,

    /* Put statistics for publish-subscribe firewall rule: payload is ATLAS_CMD_PUB_SUB_CLIENT_ID,
    ATLAS_CMD_PUB_SUB_PKT_DROP, ATLAS_CMD_PUB_SUB_PKT_PASS */
    ATLAS_CMD_PUB_SUB_PUT_STAT_FIREWALL_RULE,

    /* Publish-subscribe firewall client id: payload is client id (string)*/
    ATLAS_CMD_PUB_SUB_CLIENT_ID,

    /* Publish-subscribe firewall max QoS: payload is 1 byte QoS value */
    ATLAS_CMD_PUB_SUB_MAX_QOS,

    /* Publish-subscribe firewall packets per second: payload is 2 byte PPS value */
    ATLAS_CMD_PUB_SUB_PPS,

    /* Publish-subscribe max payload length: payload is 2 byte max payload length */
    ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN,

    /* Publish-subscribe number of dropped packets by a firewall rule: payload is 4 byte
    number of dropped packtes */
    ATLAS_CMD_PUB_SUB_PKT_DROP,

    /* Publish-subscribe number of accepted packets by a firewall rule: payload is 4 byte
    number of accepted packtes */
    ATLAS_CMD_PUB_SUB_PKT_PASS,
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_TYPE_H__ */
