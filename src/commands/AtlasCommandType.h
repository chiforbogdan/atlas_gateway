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

   /* IP command: payload is the local IP address used by gateway to connect the client*/
    ATLAS_CMD_IP_PORT,

    /* Telemetry hostname command: payload is client hostname */
    ATLAS_CMD_TELEMETRY_HOSTNAME = 500,

    /* Telemetry kernel info command: payload is client kernel info  */
    ATLAS_CMD_TELEMETRY_KERN_INFO,

    /* Telemetry sysinfo uptime command: payload is number of seconds since boot  */
    ATLAS_CMD_TELEMETRY_SYSINFO_UPTIME,

    /* Telemetry sysinfo total ram command: payload is total ram */
    ATLAS_CMD_TELEMETRY_SYSINFO_TOTALRAM,

    /* Telemetry sysinfo free ram command: payload is free ram */
    ATLAS_CMD_TELEMETRY_SYSINFO_FREERAM,

    /* Telemetry sysinfo shared ram command: payload is shared ram */
    ATLAS_CMD_TELEMETRY_SYSINFO_SHAREDRAM,

    /* Telemetry sysinfo buffer ram command: payload is buffer ram */
    ATLAS_CMD_TELEMETRY_SYSINFO_BUFFERRAM,

    /* Telemetry sysinfo total swap command: payload is total swap */
    ATLAS_CMD_TELEMETRY_SYSINFO_TOTALSWAP,

    /* Telemetry sysinfo free swap command: payload is free swap */
    ATLAS_CMD_TELEMETRY_SYSINFO_FREESWAP,

    /* Telemetry sysinfo procs command: payload is number of processes */
    ATLAS_CMD_TELEMETRY_SYSINFO_PROCS,

    /* Telemetry sysinfo total high command: payload is total high memory size */
    ATLAS_CMD_TELEMETRY_SYSINFO_TOTALHIGH,

    /* Telemetry sysinfo free high command: payload is available high memory size */
    ATLAS_CMD_TELEMETRY_SYSINFO_FREEHIGH,

    /* Telemetry sysinfo load 1 command: payload is 1 minute load average */
    ATLAS_CMD_TELEMETRY_SYSINFO_LOAD1,

    /* Telemetry sysinfo load 5 command: payload is 5 minutes load average */
    ATLAS_CMD_TELEMETRY_SYSINFO_LOAD5,

    /* Telemetry sysinfo load 15 command: payload is 15 minutes load average */
    ATLAS_CMD_TELEMETRY_SYSINFO_LOAD15,

    /* Telemetry alert external push rate: payload is a number of seconds indicating
       an interval at which data will be pushed to gateway */
    ATLAS_CMD_TELEMETRY_ALERT_EXT_PUSH_RATE,

    /* Telemetry alert internal scan rate: payload is a number of seconds indicating
       an interval at which data will be scanned internally by the client and pushed to
       gateway only if passes the a given threshold */
    ATLAS_CMD_TELEMETRY_ALERT_INT_SCAN_RATE,

    /* Telemetry alert threshold value: payload is a string indicating the threshold
       value. This string will be parsed by each telemetry feature (application specific) */
    ATLAS_CMD_TELEMETRY_ALERT_THRESHOLD,
    
    ATLAS_CMD_TELEMETRY_PACKETS_PER_MINUTE,

    ATLAS_CMD_TELEMETRY_PACKETS_AVG,


    /* COMMANDS SHARED WITH THE PUBLISH-SUBSCRIBE AGENT*/

    /* Get all publish-subscribe firewall rules: no payload */
    ATLAS_CMD_GET_ALL_PUB_SUB_FIREWALL_RULES = 1000,

    /* Install publish-subscribe firewall rule: payload is ATLAS_CMD_PUB_SUB_CLIENT_ID,
    ATLAS_CMD_PUB_SUB_MAX_QOS, ATLAS_CMD_PUB_SUB_PPM, ATLAS_CMD_PUB_SUB_MAX_PAYLOAD_LEN,*/
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

    /* Publish-subscribe firewall packets per minute: payload is 2 byte PPM value */
    ATLAS_CMD_PUB_SUB_PPM,

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
