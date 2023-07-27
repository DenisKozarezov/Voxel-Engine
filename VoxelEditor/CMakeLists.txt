cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VoxelEditor)

set_target_properties(${TARGET} PROPERTIES 
    LINKER_LANGUAGE CXX
)

file(GLOB_RECURSE EDITOR_SOURCE ./*.cpp)
file(GLOB_RECURSE EDITOR_HEADER ./*.h)

add_executable(${PROJECT_NAME} 
	${EDITOR_SOURCE}
	${EDITOR_HEADER}
)

# link VoxelEngine STATIC library
target_link_libraries(${PROJECT_NAME} PRIVATE VoxelEngine)

# ============== POST-BUILD ==============

# copy VoxelEngine.lib to executable file
add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:VoxelEngine> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copy $<TARGET_FILE:VoxelEngine> to $<TARGET_FILE_DIR:${PROJECT_NAME}>."
)

# make symbolic links to RESOURCES folder
set(RESOURCES_FOLDER "${CMAKE_SOURCE_DIR}/resources")
set(BINARY_OUTPUT_DIR "$<0:>${CMAKE_BINARY_DIR}/bin/$<CONFIG>/${PROJECT_NAME}")

# for IDE
set(link_dst "${CMAKE_CURRENT_BINARY_DIR}/resources") 
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD COMMAND ${CMAKE_COMMAND} 
        -E create_symlink ${RESOURCES_FOLDER} ${link_dst}
    DEPENDS ${link_dst}
    COMMENT "Symbolic link ${RESOURCES_FOLDER} => ${link_dst}." 
)

# for executable file
set(link_dst "${BINARY_OUTPUT_DIR}/resources")
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD COMMAND ${CMAKE_COMMAND} 
        -E create_symlink ${RESOURCES_FOLDER} ${link_dst}
    DEPENDS ${link_dst}
    COMMENT "Symbolic link ${RESOURCES_FOLDER} => ${link_dst}." 
)