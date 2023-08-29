

# if WIN32
# if(WIN32)
#         include_directories(
#                 ${SOURCE_DIR}/platforms/win
#                 ${SOURCE_DIR}/platforms/win/directX
#         )
# elseif(UNIX)
#         include_directories(
#                 ${SOURCE_DIR}/platforms/linux
#         )
# else()
#         message(FATAL_ERROR "Unsupported platform")
# endif()

# message(STATUS "Collect third party files")
# file(GLOB THIRD_PARTY_FILES 
#         ${THIRD_PARTY_DIR}/imgui/*.h
#         ${THIRD_PARTY_DIR}/imgui/*.cpp
#         ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_win32.*
#         ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_dx12.*
# )
# message(STATUS "    Collect third party files...done")

# find_package(imgui CONFIG REQUIRED)

# find_package(stb CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui stb eigen)
# target_link_libraries(${PROJECT_NAME} PRIVATE stb)

if(WIN32)
        target_link_libraries(${PROJECT_NAME}
        PUBLIC
                d3d11.lib
                dxgi.lib
                dxguid.lib
                d3dcompiler.lib
        )
elseif(UNIX)
        target_link_libraries(${PROJECT_NAME}
        PUBLIC  glfw GL dl pthread
        )
endif()


target_include_directories(
        ${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/common>
        PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/editor>
        PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/runtime>
        PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/runtime/core/math>
        PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/runtime/function>
        PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/runtime/function/renderer>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${TARGET_VERSION}>
)

if(WIN32)
        target_include_directories(
                ${PROJECT_NAME}
                PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/platforms/win>
                PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/platforms/win/directX>
                $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${TARGET_VERSION}>
        )
elseif(UNIX)
        target_include_directories(
                ${PROJECT_NAME}
                PUBLIC $<BUILD_INTERFACE:${SOURCE_DIR}/platforms/linux>
                $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${TARGET_VERSION}>
        )
else()
        message(FATAL_ERROR "Unsupported platform")
endif()