# Project name!
project(RFIDController)

# include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Boost
find_package(Boost 1.49 COMPONENTS asio system REQUIRED)
message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")

# Websocketpp
set(WEBSOCKETPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/websocketpp/websocketpp)

# include cmake compile optimisations
include(cotire)

add_subdirectory(src)
