#include <iostream>
#include "AtlasFilter.h"

namespace atlas {

AtlasFilter::AtlasFilter() {}

AtlasFilter& AtlasFilter::getInstance()
{
    static AtlasFilter instance;

    return instance;
}

void AtlasFilter::init()
{
}

} // namespace atlas
