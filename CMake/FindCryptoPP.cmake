# - Try to find LIBNFC
# Once done, this will define
#
#  CRYPTOPP_FOUND - system has CRYPTOPP
#  CRYPTOPP_INCLUDE_DIRS - the CRYPTOPP include directories
#  CRYPTOPP_LIBRARIES - link these to use CRYPTOPP

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(CRYPTOPP_PKGCONF libcrypto++ libcrypto++-dev)

# Include dir
find_path(CRYPTOPP_INCLUDE_DIR
  NAMES
    cryptlib.h
  PATHS
    ${CRYPTOPP_PKGCONF_INCLUDE_DIRS}
    /usr/include
    /usr/include/crypto++
)

# Finally the library itself
find_library(CRYPTOPP_LIBRARY
  NAMES
    crypto++
  PATHS
    ${CRYPTOPP_PKGCONF_LIBRARY_DIRS}
    /usr/lib
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(CRYPTOPP_PROCESS_INCLUDES CRYPTOPP_INCLUDE_DIR)
set(CRYPTOPP_PROCESS_LIBS CRYPTOPP_LIBRARY)
libfind_process(CRYPTOPP)
