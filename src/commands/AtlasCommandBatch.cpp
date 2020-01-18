#include <arpa/inet.h>
#include <string.h>
#include "AtlasCommandBatch.h"

namespace atlas {

void AtlasCommandBatch::parseCommands()
{
    uint8_t *ptr = cmdBuf_;

    while (cmdBufLen_ > 0) {

        /* Get command type */
        if (cmdBufLen_ < sizeof(uint16_t))
            return;

        uint16_t type = *((uint16_t *) ptr);
        type = ntohs(type);
        ptr += sizeof(uint16_t);
        cmdBufLen_ -= sizeof(uint16_t);

        /* Verify command type */

        /* Get command length */
        if (cmdBufLen_ < sizeof(uint16_t))
            return;

        uint16_t len = *((uint16_t *) ptr);
        len = ntohs(len);
        ptr += sizeof(uint16_t);
        cmdBufLen_ -= sizeof(uint16_t);

        /* Get command value */
        if (cmdBufLen_ < len)
            return;
        
        /* Save command */
        parsedCmd_.push_back(AtlasCommand(type, len, ptr));
 
        ptr += len;
        cmdBufLen_ -= len;
    }
}

void AtlasCommandBatch::setRawCommands(const uint8_t *cmd, size_t cmdLen)
{
    if (!cmd || !cmdLen)
        return;

    /* Clear existing commands, if any */
    if (cmdBuf_)
        delete[] cmdBuf_;

    parsedCmd_.clear();

    cmdBuf_ = new uint8_t[cmdLen];
    memcpy(cmdBuf_, cmd, cmdLen);
    
    cmdBufLen_ = cmdLen;

    parseCommands();
}

AtlasCommandBatch::~AtlasCommandBatch()
{
    delete[] cmdBuf_;
}

} //namespace atlas
