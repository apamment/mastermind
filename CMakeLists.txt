cmake_minimum_required(VERSION 3.6)

project(mmind)

set(CMAKE_CXX_STANDARD 11)

add_custom_target(
   odoors
   COMMAND make
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/odoors
)

add_executable(mmind main.c)
include_directories(${CMAKE_SOURCE_DIR}/odoors)
TARGET_LINK_LIBRARIES(mmind ${CMAKE_SOURCE_DIR}/odoors/libs-${CMAKE_SYSTEM_NAME}/libODoors.a)
add_dependencies(mmind odoors)