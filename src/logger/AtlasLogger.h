#ifndef __ATLAS_LOGGER_H__
#define __ATLAS_LOGGER_H__

#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#define ATLAS_LOGGER_DEBUG(MSG) BOOST_LOG_SEV(atlas::logger, boost::log::trivial::debug) << (MSG)
#define ATLAS_LOGGER_ERROR(MSG) BOOST_LOG_SEV(atlas::logger, boost::log::trivial::error) << (MSG)
#define ATLAS_LOGGER_INFO(MSG) BOOST_LOG_SEV(atlas::logger, boost::log::trivial::info) << (MSG)
#define ATLAS_LOGGER_INFO1(MSG, PARAM1) BOOST_LOG_SEV(atlas::logger, boost::log::trivial::info) << (MSG) << (PARAM1)

namespace atlas {

extern boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level> logger;

void initLog();

} // namespace atlas

#endif /* __ATLAS_LOGGER_H__ */
