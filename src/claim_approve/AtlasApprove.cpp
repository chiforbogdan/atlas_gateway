#include "AtlasApprove.h"
#include "AtlasClaim.h"
#include <json/json.h>
#include "../sql/AtlasSQLite.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"

namespace atlas {

namespace {

const std::string ATLAS_CLAIM_REQUEST_PATH = "/gateway/claim";

/* JSON fields from the incoming approve messages */
const std::string ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY = "clientIdentity";
const std::string ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY = "type";
const std::string ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY = "payload";
const std::string ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY = "identifier";

} // anonymous namespace

uint32_t AtlasApprove::sequenceNumber_ = 0; 

AtlasApprove::AtlasApprove(){}

AtlasApprove& AtlasApprove::getInstance()
{
    static AtlasApprove instance;

    return instance;
}

void AtlasApprove::checkCmdPayload(const std::string &cmdPayload)
{
    if(!AtlasClaim::getInstance().isClaimed()) {
        ATLAS_LOGGER_ERROR("Received an approved cmd for an unclaimed gateway!");
        return;
    }

    if(cmdPayload == "") {
        ATLAS_LOGGER_ERROR("Received an approved cmd with an empty payload!");
        return;
    }

    Json::Reader reader;
    Json::Value obj;

    reader.parse(cmdPayload, obj);

    if (obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Sequence number is empty!");
        return;
    }

    if (obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt() <= sequenceNumber_) {
        ATLAS_LOGGER_ERROR("Sequence number is less or equal with current sequence number!");
        return;
    } 
    sequenceNumber_ = obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();
    
    if (obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Device id is empty!");
        return;	
    }

    if (obj[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Command field is empty!");
        return;
    } 

    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists with identity " + obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
        return;
    }

    /* Save command into device Q*/
    device->addCommandClient(AtlasCommandClient(obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                                                obj[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                                                obj[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString()));

    /* Save command into the database */
    bool result = AtlasSQLite::getInstance().insertDeviceCommand(obj[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                                                                 obj[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                                                                 obj[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString(),
                                                                 obj[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if(!result) {
        ATLAS_LOGGER_ERROR("Cannot save device command into the database");
	    return;
    }
}


void AtlasApprove::RelayCmd(std::string const &cmdPayload)
{
    //CoAPs request to client
}


} // namespace atlas
