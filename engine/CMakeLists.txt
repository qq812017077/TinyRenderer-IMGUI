#print
message(STATUS "Configuring engine...")

# specify libraries
add_subdirectory(3rdparty)
# add_subdirectory(include)

# specify include directories
include_directories(
        include
        include/exceptions
        include/editor
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
                ./src/exceptions/*.cpp
                ./src/win/editor/*.cpp
                ./src/win/*.cpp
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
message(STATUS "    Configuring engine...done")