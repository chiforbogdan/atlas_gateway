#ifndef __ATLAS_CLAIM_H__
#define __ATLAS_CLAIM_H__

#include "../alarm/AtlasAlarm.h"
#include "../http/AtlasHttpServer.h"

namespace atlas {

class AtlasClaim {

public:
    /**
     * @brief Ctor for claiming protocol
     * @return none
     */
    AtlasClaim();

private:
    /**
     * @brief Short code alarm callback
     * @return none
     */
    void alarmCallback();

    AtlasHttpResponse handleClaimReq(AtlasHttpMethod method, const std::string &path,
                                     const std::string &payload);

    /* Short code alarm */
    AtlasAlarm shortCodeAlarm_;

    /* HTTP claim request callback */
    AtlasHttpCallback claimCallback_;
};

} // namespace atlas

#endif /* __ATLAS_CLAIM_H__ */
