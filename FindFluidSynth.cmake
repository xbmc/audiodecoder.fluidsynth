# - Try to find fluidsynth
# Once done this will define
#
# FLUIDSYNTH_FOUND - system has libfluidsynth
# FLUIDSYNTH_INCLUDE_DIRS - the libfluidsynth include directory
# FLUIDSYNTH_LIBRARIES - The libfluidsynth libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules (FLUIDSYNTH fluidsynth)
  list(APPEND FLUIDSYNTH_INCLUDE_DIRS ${FLUIDSYNTH_INCLUDEDIR})
endif()

if(NOT FLUIDSYNTH_FOUND)
  find_path(FLUIDSYNTH_INCLUDE_DIRS fluidsynth.h)
  find_library(FLUIDSYNTH_LIBRARIES NAMES fluidsynth)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FluidSynth DEFAULT_MSG FLUIDSYNTH_INCLUDE_DIRS FLUIDSYNTH_LIBRARIES)

mark_as_advanced(FLUIDSYNTH_INCLUDE_DIRS FLUIDSYNTH_LIBRARIES)
