#ifndef __ATLAS_FILTER_H__
#define __ATLAS_FILTER_H__

namespace atlas {

class AtlasFilter
{

public:
    static AtlasFilter& getInstance();
    void init();

    AtlasFilter(const AtlasFilter &) = delete;
    AtlasFilter &operator=(const AtlasFilter &) = delete;
private:
    AtlasFilter();
};

} // namespace atlas

#endif /* __ATLAS_FILTER_H__ */
