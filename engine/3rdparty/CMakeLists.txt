#message
message(STATUS "Configuring 3rdparty...")
set(third_party_folder "ThirdParty")

if(NOT TARGET eigen)
    set(EIGEN_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/Eigen)
    file(GLOB EIGEN_SOURCES CONFIGURE_DEPENDS  "${EIGEN_SOURCE_DIR_}/src/*.h")
    add_library(eigen INTERFACE ${EIGEN_SOURCES})
    target_include_directories(eigen INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
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
