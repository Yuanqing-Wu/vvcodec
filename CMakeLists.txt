cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(vvenc LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_LOADED)
    message(STATUS "The C++ compiler ID is: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "The C++ compiler version is: ${CMAKE_CXX_COMPILER_VERSION}")
endif()

if(CMAKE_C_COMPILER_LOADED)
    message(STATUS "The C compiler ID is: ${CMAKE_C_COMPILER_ID}")
    message(STATUS "The C compiler version is: ${CMAKE_C_COMPILER_VERSION}")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(APP_SRC ${SOURCE}/app)
set(UTILS_SRC ${SOURCE}/utils)

include_directories(${APP_SRC})
include_directories(${UTILS_SRC})

file(GLOB UTILS_FILES
    "${UTILS_SRC}/*.cpp"
    "${UTILS_SRC}/*.h"
)

add_library(utils_lib STATIC ${UTILS_FILES})

add_executable(vvenc ${APP_SRC}/vvencapp.cpp)
add_executable(vvdec ${APP_SRC}/vvdecapp.cpp)
target_link_libraries(vvenc utils_lib)
target_link_libraries(vvdec utils_lib)
