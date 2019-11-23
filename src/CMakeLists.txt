cmake_minimum_required(VERSION 2.8)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0  -g -ggdb")


message(${PROJECT_SOURCE_DIR})
set(DynamoRIO_DIR "${PROJECT_SOURCE_DIR}/../dynamorio/cmake")
find_package(DynamoRIO)


if (NOT DynamoRIO_FOUND)
  message(FATAL_ERROR "DynamoRIO package required to build")
endif ()

add_library(strace SHARED strace.c xlat.c)

configure_DynamoRIO_client(strace)
use_DynamoRIO_extension(strace drmgr)
use_DynamoRIO_extension(strace drsyscall)
