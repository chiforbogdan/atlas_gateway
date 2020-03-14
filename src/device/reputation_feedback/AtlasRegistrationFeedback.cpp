#include "AtlasRegistrationFeedback.h"
#include "../AtlasDeviceManager.h"
#include "../../logger/AtlasLogger.h"

namespace atlas {

double AtlasRegistrationFeedback::getFeedback()
{
    int regIntervalSec = device_.getRegInterval();
    
    /* Compute the registration time percentage */
    double feedback = (double) (regIntervalSec - prevRegIntervalSec_) / (double) samplePeriodSec_; 

    prevRegIntervalSec_ = regIntervalSec;

    ATLAS_LOGGER_INFO("Feedback was computed for registration time (system reputation)");

    return feedback;
}

} // namespace atlas
