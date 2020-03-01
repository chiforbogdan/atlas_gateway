#ifndef __ATLAS_COMMANDS_CLOUD_H__
#define __ATLAS_COMMANDS_CLOUD_H__

namespace atlas {

/* Cloud command structure */
const std::string ATLAS_CMD_TYPE_JSON_KEY = "commandType";
const std::string ATLAS_CMD_PAYLOAD_JSON_KEY = "commandPayload";

/* Cloud command type */
const std::string ATLAS_CMD_CLIENT_INFO_UPDATE = "ATLAS_CMD_CLIENT_INFO_UPDATE";
const std::string ATLAS_CMD_GATEWAY_REGISTER = "ATLAS_CMD_GATEWAY_REGISTER";
const std::string ATLAS_CMD_GATEWAY_KEEPALIVE = "ATLAS_CMD_GATEWAY_KEEPALIVE";

} // namespace atlas


#endif /* __ATLAS_COMMANDS_CLOUD_H__ */
