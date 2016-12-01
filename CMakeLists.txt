cmake_minimum_required(VERSION 3.0)

# Project name!
project(RFIDController CXX)

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

# Boost
find_package(Boost 1.49 COMPONENTS system REQUIRED)
message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")

# LIBNFC
find_package(LIBNFC REQUIRED)

# LIBFREEFARE
find_package(LIBFREEFARE REQUIRED)

# Websocketpp
set(WEBSOCKETPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/websocketpp/websocketpp)

# Include cmake compile optimisations
include(cotire)

# Enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Include sources
add_subdirectory(src)
