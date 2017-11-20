# - Try to find ZeroMQ include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(ZeroMQ)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  ZEROMQ_ROOT_DIR           Set this variable to the root installation of
#                            ZeroMQ if the module has problems finding
#                            the proper installation path.
#
# Variables defined by this module:
#
#  ZEROMQ_FOUND              System has ZeroMQ dev headers/libraries
#  ZEROMQ_INCLUDE_DIR        The ZeroMQ include directories.
#  ZEROMQ_LIBRARIES          The ZeroMQ libraries and linker flags.

find_path(ZEROMQ_ROOT_DIR
    NAMES include/zmq.h
)

find_library(ZEROMQ_LIBRARIES
    NAMES zmq
    HINTS ${ZEROMQ_ROOT_DIR}/lib
    PATH_SUFFIXES ${CMAKE_LIBRARY_ARCHITECTURE}
)

find_path(ZEROMQ_INCLUDE_DIR
    NAMES include/zmq.h
    HINTS ${ZEROMQ_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZeroMQ DEFAULT_MSG
    ZEROMQ_LIBRARIES
    ZEROMQ_INCLUDE_DIR
)

mark_as_advanced(
    ZEROMQ_ROOT_DIR
    ZEROMQ_LIBRARIES
    ZEROMQ_INCLUDE_DIR
)
