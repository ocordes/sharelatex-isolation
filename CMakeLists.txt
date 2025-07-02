cmake_minimum_required(VERSION 3.5)
# minimal CMake configuration for sharelatex-isolation library

project(sharelatex-isolation VERSION 0.0.1 DESCRIPTION "sharelatex-isolation library")

set(VERSION 0.0.1)

file(GLOB_RECURSE sources src/*.c src/*.h)

configure_file(src/config.h.in ${PROJECT_BINARY_DIR}/config.h)
# Now make sure that you the the build directory on your "Include" path when compiling
# so that the generated config.h is found.
# This is necessary because the config.h file is generated during the CMake configuration step,
# and the source files are compiled during the build step.
# The build directory is not the same as the source directory, so you need to include the
# build directory in your include path.
# This is done by the following line:
include_directories(${PROJECT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/whitelist)


add_library(shareiso SHARED ${sources})


set_target_properties(shareiso PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}
                      OUTPUT_NAME shareiso)

