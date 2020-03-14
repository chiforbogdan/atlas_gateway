#include "AtlasKeepaliveFeedback.h"
#include "../AtlasDeviceManager.h"
#include "../../logger/AtlasLogger.h"

namespace atlas {

double AtlasKeepaliveFeedback::getFeedback()
{
    int keepalivePkts = device_.getKeepalivePkts();
    
    /* Compute the keep-alive packets percentage rate */
    double feedback = (double) (keepalivePkts - prevKeepalivePkts_) / (double) sampleKeepalivePkts_; 

    prevKeepalivePkts_ = keepalivePkts;

    ATLAS_LOGGER_INFO("Feedback was computed for keep-alive packets (system reputation)");

    return feedback;
}

} // namespace atlas
