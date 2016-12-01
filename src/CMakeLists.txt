
file(GLOB sources_localdir *.cpp *.h)

set(sources_RFIDCtrl
  ${sources_localdir}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${WEBSOCKETPP_INCLUDE_DIR}
)

# Create executable
add_executable(RFIDCtrl
  ${sources_RFIDCtrl}
)
