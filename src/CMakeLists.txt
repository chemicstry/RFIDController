
file(GLOB sources_PCH PCH/*.cpp PCH/*.h)
file(GLOB sources_localdir *.cpp *.h)

set(sources_RFIDCtrl
    ${sources_PCH}
    ${sources_localdir}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/PCH
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${WEBSOCKETPP_INCLUDE_DIR}
    ${JSON_INCLUDE_DIR}
    ${LIBNFC_INCLUDE_DIR}
    ${LIBFREEFARE_INCLUDE_DIR}
    ${CRYPTOPP_INCLUDE_DIR}
)

# Create executable
add_executable(RFIDCtrl
    ${sources_RFIDCtrl}
)

# Link libraries
target_link_libraries(RFIDCtrl
    ${LIBNFC_LIBRARY}
    ${LIBFREEFARE_LIBRARY}
    ${CRYPTOPP_LIBRARY}
    ${Boost_LIBRARIES}
    pthread
)

# Set PCH
set_target_properties(RFIDCtrl PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "PCH/PCH.h")

# Optimise compilation
cotire(RFIDCtrl)
