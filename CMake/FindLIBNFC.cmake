# - Try to find LIBNFC
# Once done, this will define
#
#  LIBNFC_FOUND - system has libnfc
#  LIBNFC_INCLUDE_DIRS - the libnfc include directories
#  LIBNFC_LIBRARIES - link these to use libnfc

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(LIBNFC_PKGCONF libnfc libnfc-dev)

# Include dir
find_path(LIBNFC_INCLUDE_DIR
  NAMES
    nfc.h
  PATHS
    ${LIBNFC_PKGCONF_INCLUDE_DIRS}
    /usr/include
    /usr/include/nfc
)

# Finally the library itself
find_library(LIBNFC_LIBRARY
  NAMES
    nfc
  PATHS
    ${LIBNFC_PKGCONF_LIBRARY_DIRS}
    /usr/lib
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(LIBNFC_PROCESS_INCLUDES LIBNFC_INCLUDE_DIR)
set(LIBNFC_PROCESS_LIBS LIBNFC_LIBRARY)
libfind_process(LIBNFC)
