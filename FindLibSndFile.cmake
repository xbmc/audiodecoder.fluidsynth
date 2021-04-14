# - Try to find fluidsynth
# Once done this will define
#
# LIBSNDFILE_FOUND - system has libsndfile
# LIBSNDFILE_INCLUDE_DIRS - the libsndfile include directory
# LIBSNDFILE_LIBRARIES - The libsndfile libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_LIBSNDFILE sndfile QUIET)
endif()

find_path(LIBSNDFILE_INCLUDE_DIRS sndfile.h
                                  PATHS ${PC_LIBSNDFILE_INCLUDEDIR})
find_library(LIBSNDFILE_LIBRARIES sndfile
                                  PATHS ${PC_LIBSNDFILE_LIBDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibSndFile REQUIRED_VARS LIBSNDFILE_INCLUDE_DIRS LIBSNDFILE_LIBRARIES)

mark_as_advanced(LIBSNDFILE_INCLUDE_DIRS LIBSNDFILE_LIBRARIES)
