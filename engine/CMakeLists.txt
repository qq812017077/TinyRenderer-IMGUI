#print
message(STATUS "Configuring engine...")

# specify libraries
add_subdirectory(3rdparty)
# add_subdirectory(include)

# specify include directories
include_directories(
        include
        include/editor
        include/inputs
        include/exceptions
        include/graphics
        include/tools
        3rdparty/imgui
        3rdparty/imgui/backends
)

# set common source files
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB THIRD_PARTY 
        ./3rdparty/imgui/*.h
        ./3rdparty/imgui/*.cpp
        ./3rdparty/imgui/backends/imgui_impl_win32.*
        ./3rdparty/imgui/backends/imgui_impl_dx12.*
)
if(WIN32)
        include_directories(
                include/editor/win
                include/graphics/win
        )
        
        file(GLOB INCLUDE_PLATFORM
                ./include/exceptions/*.h
                ./include/editor/win/*.h
                ./include/graphics/win/*.h
                ./include/inputs/*.h
                ./include/tools/*.h
                ./include/*.h
        )

        file(GLOB SRC_PLATFORM
                ./src/exceptions/*.cpp
                ./src/editor/win/*.cpp
                ./src/graphics/win/*.cpp
                ./src/tools/*.cpp
                ./src/*.cpp
        )
elseif(UNIX)
        file(GLOB INCLUDE_PLATFORM
                ./include/exceptions/*.h
                ./include/inputs/*.h
                ./include/tools/*.h
                ./include/*.h
        )
        file(GLOB SRC_PLATFORM
                ./3rdparty/imgui/backends/imgui_impl_glfw.*
                ./3rdparty/imgui/backends/imgui_impl_opengl3.*
                ./src/linux/main.cpp
        )
endif()

source_group(TREE ${INCLUDE_DIR} PREFIX "Header Files" FILES ${INCLUDE_PLATFORM})
source_group(TREE ${SOURCE_DIR} PREFIX "Source Files" FILES ${SRC_PLATFORM})
source_group(TREE ${THIRD_PARTY_DIR} FILES ${THIRD_PARTY})

# specify executable
add_executable(${PROJECT_NAME} ${THIRD_PARTY}  ${SRC_PLATFORM} ${INCLUDE_PLATFORM})

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