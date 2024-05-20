
cmake_minimum_required(VERSION 3.15)

project(ZeekPluginZeroMQWriter)

include(ZeekPlugin)

find_package(ZeroMQ)

if (NOT ZEROMQ_FOUND)
    message(FATAL_ERROR "Cannot find ZeroMQ, use --with-zmq=DIR.")
endif ()

include_directories(BEFORE ${ZEROMQ_INCLUDE_DIR})
zeek_plugin_begin(NCSA ZeroMQWriter)
zeek_plugin_cc(src/Plugin.cc)
zeek_plugin_cc(src/ZeroMQ.cc)
zeek_plugin_bif(src/zeromqwriter.bif)
zeek_plugin_dist_files(README CHANGES COPYING VERSION)
zeek_plugin_link_library(${ZEROMQ_LIBRARIES})
zeek_plugin_end()

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION LIMIT_COUNT 1)

if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    # Allows building rpm/deb packages via "make package" in build dir.
    include(ConfigurePackaging)
    ConfigurePackaging(${VERSION})
endif ()
