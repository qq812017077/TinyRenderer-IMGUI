cmake_minimum_required(VERSION 3.10)
project(imgui)

file(GLOB imgui_sources CONFIGURE_DEPENDS  "${IMGUI_SOURCE_DIR_}/*.cpp")
# if Win32
if (WIN32)
    file(GLOB imgui_impl CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_dx11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_dx11.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_win32.h")
elseif(UNIX)
    file(GLOB imgui_impl CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_vulkan.h")
endif()
    add_library(imgui STATIC ${imgui_sources} ${imgui_impl})
    target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})