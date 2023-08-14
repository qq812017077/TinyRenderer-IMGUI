#print
message(STATUS "Configuring engine...")

# set(TARGET_NAME EngineSource)

# set common source files
set(SOURCE_DIR ${ENGINE_ROOT_DIR}/source)

# collect shader files
message(STATUS "Collect shader files")
file(GLOB_RECURSE SHADER_FILES ${SHADER_DIR}/*.hlsl)

# collect source files
message(STATUS "Collect source files")
file(GLOB_RECURSE COMMON_INCLUDE_FILES ${SOURCE_DIR}/editor/*.h ${SOURCE_DIR}/runtime/*.h ${SOURCE_DIR}/common/*.h)
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
# source_group(TREE ${THIRD_PARTY_DIR} PREFIX "3rdparty" FILES ${THIRD_PARTY_FILES})
source_group(TREE ${SHADER_DIR} PREFIX "shaders" FILES ${SHADER_FILES})
message(STATUS "    set source group...done")

# specify executable
add_executable(${PROJECT_NAME} ${INCLUDE_FILES} ${SOURCE_FILES})

# target_link_libraries(${PROJECT_NAME} PRIVATE stb)
# link dependencies
include(link_dependencies.cmake)

# clear dir and copy shader files
message(STATUS "Clear dir and copy shader files")
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/Debug/shaders)
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/shaders)
message(STATUS "    Configuring engine...done")