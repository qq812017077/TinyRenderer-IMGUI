#message
message(STATUS "Configuring 3rdparty...")
set(third_party_folder "ThirdParty")

# if(not TARGET imgui)
#     add_subdirectory(imgui)
#     # specify include directories
#     include_directories(
#         ./imgui
#         ./imgui/backends
#         ./stb
#     )
# endif()

if(NOT TARGET stb)
    file(GLOB STB_SOURCES CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h")
    add_library(stb INTERFACE ${STB_SOURCES})
    target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()
message(STATUS "    Configuring 3rdparty...done")
