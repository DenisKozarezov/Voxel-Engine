cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VoxelEngine)
add_subdirectory(ThirdParty)
add_subdirectory(VoxelEngine)
add_subdirectory(VoxelEditor)

# ================ BUILD DIRECTORIES ================
set(LIBRARY_OUTPUT_DIR "$<0:>${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(BINARY_OUTPUT_DIR "$<0:>${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

set_target_properties(VoxelEngine PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR}/VoxelEngine
	LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR}/VoxelEngine
	RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR}/VoxelEngine
)
set_target_properties(VoxelEditor PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR}/VoxelEditor
	EXECUTABLE_OUTPUT_PATH ${BINARY_OUTPUT_DIR}/VoxelEditor
)