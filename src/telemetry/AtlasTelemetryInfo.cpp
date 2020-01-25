#include "AtlasTelemetryInfo.h"

namespace atlas {

std::string AtlasTelemetryInfo::toString()
{
    std::string featureString = "\n{\n";

    std::unordered_map<std::string, std::string>::iterator it = features_.begin();
    while (it != features_.end()) {
        featureString += "\"" + (*it).first + "\":\"" + (*it).second + "\"";
        ++it;
        if (it != features_.end())
            featureString += ",\n";
        else
            featureString += "\n";
    }

    featureString += "}";

    return featureString;
}

} // namespace atlas
