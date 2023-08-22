cmake_minimum_required(VERSION 3.10)
project(stb)

file(GLOB STB_SOURCES CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h")
add_library(stb INTERFACE ${STB_SOURCES})
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
# set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)