#message
message(STATUS "Configuring 3rdparty...")
set(third_party_folder "ThirdParty")

if(NOT TARGET eigen)
    set(EIGEN_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/Eigen)
    file(GLOB EIGEN_SOURCES CONFIGURE_DEPENDS  "${EIGEN_SOURCE_DIR_}/src/*.h")
    add_library(eigen INTERFACE ${EIGEN_SOURCES})
    target_include_directories(eigen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT TARGET assimp)
    set(ASSIMP_BUILD_ZLIB ON)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
    set(ASSIMP_BUILD_TESTS OFF)
    set(ASSIMP_INSTALL OFF)
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
    add_subdirectory("assimp")
    # set(ASSIMP_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/assimp)
    # file(GLOB ASSIMP_SOURCES CONFIGURE_DEPENDS  "${ASSIMP_SOURCE_DIR_}/include/*.h" "${ASSIMP_SOURCE_DIR_}/include/*.hpp")
    # add_library(assimp INTERFACE ${ASSIMP_SOURCES})
    # target_include_directories(assimp INTERFACE ${ASSIMP_SOURCE_DIR_}/include)
endif()

if(NOT TARGET imgui)
    # add_subdirectory(imgui)
    set(IMGUI_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
    file(GLOB imgui_sources CONFIGURE_DEPENDS  "${IMGUI_SOURCE_DIR_}/*.cpp")
# if Win32
if (WIN32)
    file(GLOB imgui_impl CONFIGURE_DEPENDS
    "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_dx11.cpp"
    "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_dx11.h"
    "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_win32.cpp"
    "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_win32.h")
elseif(UNIX)
    file(GLOB imgui_impl CONFIGURE_DEPENDS
    "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_vulkan.cpp"
    "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_vulkan.h")
endif()
    add_library(imgui STATIC ${imgui_sources} ${imgui_impl})
    target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT TARGET stb)
    file(GLOB STB_SOURCES CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/stb/*.h")
    add_library(stb INTERFACE ${STB_SOURCES})
    target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

message(STATUS "    Configuring 3rdparty...done")
