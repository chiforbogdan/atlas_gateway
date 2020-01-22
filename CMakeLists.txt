cmake_minimum_required(VERSION 2.8)

project(atlas_gateway)

include(FindPkgConfig)
# CoAP dependency
pkg_check_modules(COAP libcoap-2-openssl REQUIRED)

# Link with dynamic version of Boost (for logging)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package(Boost 1.65.0 REQUIRED system log)

include_directories(include)
# Add CoAP headers
include_directories(${COAP_INCLUDE_DIRS})

# Atlas gateway files
file(GLOB ATLAS_GATEWAY_SOURCES "src/*.cpp" "src/scheduler/*.cpp" "src/coap/*.cpp" "src/logger/*.cpp" "src/alarm/*.cpp" "src/register/*.cpp" "src/commands/*.cpp" "src/pubsub_agent/*.cpp")

link_directories(${COAP_LIBRARY_DIRS})
add_executable(${PROJECT_NAME} ${ATLAS_GATEWAY_SOURCES})

#Add CoAP and Boost library
target_link_libraries(${PROJECT_NAME} ${COAP_LIBRARIES} ${Boost_LIBRARIES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

# Mosquitto dependency
pkg_check_modules(MOSQUITTO libmosquitto REQUIRED)

# Atlas mosquitto plug-in files
file(GLOB ATLAS_GATEWAY_MOSQUITTO_PLUGIN_SOURCES "src/mosquitto_plugin/*.cpp" "src/mosquitto_plugin/logger/*.cpp" "src/mosquitto_plugin/filter/*.cpp")

add_library(atlas_gateway_mosquitto_plugin SHARED ${ATLAS_GATEWAY_MOSQUITTO_PLUGIN_SOURCES})
target_link_libraries(${PROJECT_NAME}_mosquitto_plugin ${MOSQUITTO_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
