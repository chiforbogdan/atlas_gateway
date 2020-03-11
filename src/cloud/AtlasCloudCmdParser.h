#ifndef __ATLAS_CLOUD_CMD_PARSER_H__
#define __ATLAS_CLOUD_CMD_PARSER_H__

#include "../alarm/AtlasAlarm.h"
#include <boost/function.hpp>
#include <jsoncpp/json/json.h>
#include <unordered_map>

namespace atlas {

class AtlasCloudCmdParser
{
public:
    /**
    * @brief Get instance for cloud control module
    * @return Cloud control module instance
    */
    static AtlasCloudCmdParser& getInstance();

    /**
    * @brief Start cloud control module
    * @return none
    */
    void start();

    /**
    * @brief Parse a received command
    * @param[in] command
    * @return none
    */
    void parseCmd(std::string const &cmd);

    AtlasCloudCmdParser(const AtlasCloudCmdParser &) = delete;
    AtlasCloudCmdParser& operator=(const AtlasCloudCmdParser &) = delete;

private:
    /**
    * @brief Default ctor for cloud control module
    * @return none
    */
    AtlasCloudCmdParser();

    /**
    * @brief Register a callback for a command type
    * @param[in] command type
    * @param[in] callback function
    * @return none
    */
    void addCmdTypeCallback(std::string const &cmdType, std::function<void()> callback);

    /**
    * @brief Erase a callback linked with a command type
    * @param[in] command type
    * @return none
    */
    void delCmdTypeCallback(std::string const &cmdType);

    /**
    * @brief Callback for ATLAS_CMD_GET_ALL_DEVICES command
    * @return none
    */
    void CommandGetAllDevicesCallback();

    /**
    * @brief Callback for subscribe event
    * @return none
    */
    void connectAlarmCb();

    /* Indicates if the cloud module is connected */
    bool connected_;

    /* Alarm for subscribe event*/
    AtlasAlarm cAlarm_;

    /*Registered callbacks for commands type*/
    /* <cmd_type, callback function>*/
    std::unordered_map<std::string, std::function<void()>> callbacks_;
};

} // namespace atlas

#endif /* __ATLAS_CLOUD_CMD_PARSER_H__ */
