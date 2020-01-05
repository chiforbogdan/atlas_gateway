cmake_minimum_required(VERSION 2.8)

project(atlas_gateway)

include(FindPkgConfig)
# CoAP dependency
pkg_check_modules(COAP libcoap-2-openssl REQUIRED)

find_package(Boost 1.65.0 REQUIRED system)

include_directories(include)
# Add CoAP headers
include_directories(${COAP_INCLUDE_DIRS})

# Atlas client files
file(GLOB SOURCES "src/*.cpp" "src/scheduler/*.cpp" "src/coap/*.cpp")

link_directories(${COAP_LIBRARY_DIRS})
add_executable(${PROJECT_NAME} ${SOURCES})

#Add CoAP and Boost library
target_link_libraries(${PROJECT_NAME} ${COAP_LIBRARIES} ${Boost_LIBRARIES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
