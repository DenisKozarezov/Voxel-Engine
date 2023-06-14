cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VoxelEditor)

include_directories(${CMAKE_SOURCE_DIR}/VoxelEngine/src)
set_target_properties(${TARGET} PROPERTIES 
    LINKER_LANGUAGE CXX
)

add_executable(${PROJECT_NAME} 
	src/EditorApplication.cpp
	src/EditorLayer.cpp
	src/EditorLayer.h
)

# link VoxelEngine SHARED library
find_library(VoxelEngine NAMES VoxelEngine VoxelEngine_dll
    PATHS RUNTIME_OUTPUT_DIRECTORY)
target_link_libraries(${PROJECT_NAME} PRIVATE VoxelEngine)

add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:VoxelEngine> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)