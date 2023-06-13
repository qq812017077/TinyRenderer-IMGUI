#print
message(STATUS "Configuring engine...")

# set common source files
set(SOURCE_DIR ${ENGINE_ROOT_DIR}/source)


# specify include directories and link directories
include_directories(
        ${SOURCE_DIR}/editor
        ${SOURCE_DIR}/platforms
        ${SOURCE_DIR}/runtime/function/input
        ${SOURCE_DIR}/runtime/function/renderer
        ${SOURCE_DIR}/runtime/function/ui
        ${SOURCE_DIR}/runtime/core
        ${THIRD_PARTY_DIR}/imgui
        ${THIRD_PARTY_DIR}/imgui/backends
)

# if WIN32
if(WIN32)
        include_directories(
                ${SOURCE_DIR}/platforms/win
                ${SOURCE_DIR}/platforms/win/directX
        )
elseif(UNIX)
        include_directories(
                ${SOURCE_DIR}/platforms/linux
        )
else()
        message(FATAL_ERROR "Unsupported platform")
endif()

message(STATUS "Collect third party files")
file(GLOB THIRD_PARTY_FILES 
        ${THIRD_PARTY_DIR}/imgui/*.h
        ${THIRD_PARTY_DIR}/imgui/*.cpp
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_win32.*
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx12.*
)
message(STATUS "    Collect third party files...done")

# collect source files
message(STATUS "Collect source files")
file(GLOB_RECURSE COMMON_INCLUDE_FILES ${SOURCE_DIR}/editor/*.h ${SOURCE_DIR}/runtime/*.h)
file(GLOB_RECURSE COMMON_SOURCE_FILES ${SOURCE_DIR}/editor/*.cpp ${SOURCE_DIR}/runtime/*.cpp)
if(WIN32)
        file(GLOB_RECURSE PLATFORM_INCLUDE_FILES ${SOURCE_DIR}/platforms/win/*.h)
        file(GLOB_RECURSE PLATFORM_SOURCE_FILES ${SOURCE_DIR}/platforms/win/*.cpp)
elseif(UNIX)
        file(GLOB PLATFORM_INCLUDE_FILES ${SOURCE_DIR}/platforms/linux/*.h)
        file(GLOB PLATFORM_SOURCE_FILES ${SOURCE_DIR}/platforms/linux/*.cpp)
else()
        message(FATAL_ERROR "Unsupported platform")
endif()
file(GLOB INCLUDE_FILES ${COMMON_INCLUDE_FILES} ${PLATFORM_INCLUDE_FILES})
file(GLOB SOURCE_FILES ${COMMON_SOURCE_FILES} ${PLATFORM_SOURCE_FILES})
message(STATUS "    Collect source files...done")


# set source group
message(STATUS "set source group")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${INCLUDE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCE_FILES})
source_group(TREE ${THIRD_PARTY_DIR} PREFIX "3rdparty" FILES ${THIRD_PARTY_FILES})
message(STATUS "    set source group...done")

# specify executable
add_executable(${PROJECT_NAME} ${THIRD_PARTY_FILES} ${INCLUDE_FILES} ${SOURCE_FILES})

# link lib
if(WIN32)
        target_link_libraries(${PROJECT_NAME}
                d3d12.lib
                dxgi.lib
                dxguid.lib
                d3dcompiler.lib
        )
elseif(UNIX)
        target_link_libraries(${PROJECT_NAME}
                glfw
                GL
                dl
                pthread
        )
endif()
message(STATUS "    Configuring engine...done")