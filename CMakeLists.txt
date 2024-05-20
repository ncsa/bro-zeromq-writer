
cmake_minimum_required(VERSION 3.15)

project(BroPluginZeroMQWriter)

include(BroPlugin)

find_package(ZeroMQ)

if (NOT ZEROMQ_FOUND)
    message(FATAL_ERROR "Cannot find ZeroMQ, use --with-zmq=DIR.")
endif ()

include_directories(BEFORE ${ZEROMQ_INCLUDE_DIR})
bro_plugin_begin(NCSA ZeroMQWriter)
bro_plugin_cc(src/Plugin.cc)
bro_plugin_cc(src/ZeroMQ.cc)
bro_plugin_bif(src/zeromqwriter.bif)
bro_plugin_dist_files(README CHANGES COPYING VERSION)
bro_plugin_link_library(${ZEROMQ_LIBRARIES})
bro_plugin_end()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION LIMIT_COUNT 1)

if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    # Allows building rpm/deb packages via "make package" in build dir.
    include(ConfigurePackaging)
    ConfigurePackaging(${VERSION})
endif ()
