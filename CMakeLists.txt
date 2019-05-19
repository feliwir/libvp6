cmake_minimum_required(VERSION 3.10)
set(VP6_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(VP6_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)

project(libvp6 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Enable tests" OFF)
option(BUILD_TOOLS "Enable tools" OFF)
option(BUILD_SHARED_LIBS "Enable shared library" OFF)

add_subdirectory(src)

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
