cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Editor)

set_target_properties(${TARGET} PROPERTIES 
    LINKER_LANGUAGE CXX
)

file(GLOB_RECURSE EDITOR_SOURCE ./*.cpp)
file(GLOB_RECURSE EDITOR_HEADER ./*.h)

add_executable(${PROJECT_NAME} 
	${EDITOR_SOURCE}
	${EDITOR_HEADER}
)

# link Runtime STATIC library
target_link_libraries(${PROJECT_NAME} PRIVATE Runtime)

# ============== POST-BUILD ==============

# copy Runtime.lib to executable file
add_custom_command (TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Runtime> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copy $<TARGET_FILE:Runtime> to $<TARGET_FILE_DIR:${PROJECT_NAME}>."
)

# make symbolic links to RESOURCES folder
set(RESOURCES_NAME "Resources")
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/${RESOURCES_NAME}")
set(BINARY_OUTPUT_DIR "$<0:>${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/${PROJECT_NAME}")

# for IDE
set(link_dst "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_NAME}")
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD COMMAND ${CMAKE_COMMAND} 
        -E create_symlink ${RESOURCES_DIR} ${link_dst}
    DEPENDS ${link_dst}
    COMMENT "Symbolic link ${RESOURCES_DIR} => ${link_dst}." 
)

# for executable file
set(link_dst "${BINARY_OUTPUT_DIR}/${RESOURCES_NAME}")
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD COMMAND ${CMAKE_COMMAND} 
        -E create_symlink ${RESOURCES_DIR} ${link_dst}
    DEPENDS ${link_dst}
    COMMENT "Symbolic link ${RESOURCES_DIR} => ${link_dst}." 
)