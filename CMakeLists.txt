#
# This is a CMake makefile.  You can find the cmake utility and
# information about it at http://www.cmake.org
#

cmake_minimum_required(VERSION 2.8.4)

PROJECT(transito-cv)

include(dlib/cmake)

set(CMAKE_BUILD_TYPE Release)
set(USE_AVX_INSTRUCTIONS 1)

MACRO(add_source name)
    ADD_EXECUTABLE(${name} src/${name}.cpp)
    TARGET_LINK_LIBRARIES(${name} dlib )
ENDMACRO()

add_source(hog_detector)
add_source(train_object_detector)
add_source(detect)
add_source(view_hog)

