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
set(CMAKE_CXX_STANDARD 11)

# specify libraries
add_subdirectory(3rdparty)

# specify include directories
include_directories(
        include
        3rdparty/imgui
        3rdparty/imgui/backends
)

# set common source files
file(GLOB SRC 
        ./3rdparty/imgui/*.h
        ./3rdparty/imgui/*.cpp        
)

if(WIN32)
        file(GLOB SRC_PLATFORM
                ./3rdparty/imgui/backends/imgui_impl_win32.*
                ./3rdparty/imgui/backends/imgui_impl_dx12.*
                ./src/win/main.cpp
        )
elseif(UNIX)
        file(GLOB SRC_PLATFORM
                ./3rdparty/imgui/backends/imgui_impl_glfw.*
                ./3rdparty/imgui/backends/imgui_impl_opengl3.*
                ./src/linux/main.cpp
        )
endif()

# specify executable
add_executable(${PROJECT_NAME} ${SRC} ${SRC_PLATFORM})

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