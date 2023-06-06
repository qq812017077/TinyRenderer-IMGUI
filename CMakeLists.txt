# specify CMAKE version
cmake_minimum_required(VERSION 3.10)

#print
message(STATUS "Configuring project...")
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message(STATUS "CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS " ")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# specify project name
project(TinyRenderer_IMGUI)
set(PROJECT_NAME tinyrenderer)

# specify C++ standard
set(CMAKE_CXX_STANDARD 17)

#add subdirectory
add_subdirectory(engine)