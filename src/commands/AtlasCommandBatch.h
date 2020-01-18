#ifndef __ATLAS_COMMAND_BATCH_H__
#define __ATLAS_COMMAND_BATCH_H__

#include <vector>
#include "AtlasCommand.h"

namespace atlas {

class AtlasCommandBatch
{

public:
    /**
    * @brief Ctor for command batch
    * @return none
    */
    AtlasCommandBatch() : cmdBuf_(nullptr), cmdBufLen_(0) {}

    /**
    * @brief Provide the command batch a raw command byte array
    * @param[in] cmd Raw command byte array
    * @param[in] cmdLen Raw command byte array length
    * @return none
    */
    void setRawCommands(const uint8_t *cmd, size_t cmdLen);

    /**
    * @brief Get parsed commands
    * @return Parsed commands
    */
    std::vector<AtlasCommand> getParsedCommands() const { return parsedCmd_; }

    /**
    * @brief Dtor for command batch
    * @return none
    */
    ~AtlasCommandBatch();
private:
    /**
    * @brief Parse raw commands
    * @return none
    */
    void parseCommands();

    /* Raw commands byte array */
    uint8_t *cmdBuf_;
    
    /* Raw commands byte array length */
    size_t cmdBufLen_;
    
    /* Parsed commands list */
    std::vector<AtlasCommand> parsedCmd_;
};

} // namespace atlas

#endif /* __ATLAS_COMMAND_BATCH_H__ */
