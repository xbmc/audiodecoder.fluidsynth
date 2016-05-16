project(audiodecoder.fluidsynth)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)
find_package(FluidSynth REQUIRED)

include(ExternalProject)

include_directories(${KODI_INCLUDE_DIR}
                    ${FLUIDSYNTH_INCLUDE_DIRS})

set(FLUID_SOURCES src/FluidDecoder.cpp)

set(DEPLIBS ${FLUIDSYNTH_LIBRARIES})

build_addon(audiodecoder.fluidsynth FLUID DEPLIBS)

include(CPack)
