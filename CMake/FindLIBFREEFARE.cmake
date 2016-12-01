# - Try to find LIBNFC
# Once done, this will define
#
#  LIBFREEFARE_FOUND - system has libfreefare
#  LIBFREEFARE_INCLUDE_DIRS - the libfreefare include directories
#  LIBFREEFARE_LIBRARIES - link these to use libfreefare

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(LIBFREEFARE_PKGCONF libfreefare libfreefare-dev)

# Include dir
find_path(LIBFREEFARE_INCLUDE_DIR
  NAMES
    freefare.h
  PATHS
    ${LIBFREEFARE_PKGCONF_INCLUDE_DIRS}
    /usr/include
    /usr/include/nfc
)

# Finally the library itself
find_library(LIBFREEFARE_LIBRARY
  NAMES
    freefare
  PATHS
    ${LIBFREEFARE_PKGCONF_LIBRARY_DIRS}
    /usr/lib
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(LIBFREEFARE_PROCESS_INCLUDES LIBFREEFARE_INCLUDE_DIR)
set(LIBFREEFARE_PROCESS_LIBS LIBFREEFARE_LIBRARY)
libfind_process(LIBFREEFARE)
