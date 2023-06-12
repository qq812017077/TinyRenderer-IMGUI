# specify CMAKE version
cmake_minimum_required(VERSION 3.10)


project(TinyRenderer_IMGUI VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(CMAKE_INSTALL_PREFIX "${PROJECT_ROOT_DIR}/bin")
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")
set(ENGINE_ROOT_DIR "${PROJECT_ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")

message(STATUS "Configuring project...")
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS " ")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# specify project name
set(PROJECT_NAME tinyrenderer)
# specify C++ standard
set(CMAKE_CXX_STANDARD 17)

#add subdirectory
add_subdirectory(3rdparty)
add_subdirectory(source)