cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VoxelEngine VERSION 0.0.1)

set(PUBLIC_HEADERS src/pch.h)

set_target_properties(${TARGET} PROPERTIES 
    PUBLIC_HEADER ${PUBLIC_HEADERS}
    LINKER_LANGUAGE CXX
)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE CORE_SOURCE ./src/*.cpp)
file(GLOB_RECURSE CORE_HEADER ./src/*.h)

source_group("core" FILES ${CORE_SOURCE})
source_group("core" FILES ${CORE_HEADER})

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

configure_file (version.h.in ${SOURCE_DIR}/version.h @ONLY)

add_library(${PROJECT_NAME} 
    STATIC 
        ${CORE_SOURCE}
        ${CORE_HEADER}
        ${SOURCE_DIR}/VoxelEngine.h
        ${SOURCE_DIR}/pch.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_DIR})

# ============= PREPROCESSOR DEFINES ==============
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} 
        PRIVATE
            /W4         # Warning Level 4
        PUBLIC
            /MP         # Multi-Processor Compilation
            /sdl-       # No SDL Check
            /fp:fast    # Fast Floating-Point
            /GS-        # No Security Checks
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PUBLIC        
        VOXELENGINE_EXPORTS
        $<$<CONFIG:Debug>:VOXEL_DEBUG>
        $<$<CONFIG:RelWithDebInfo>:VOXEL_DEBUG>
        $<$<CONFIG:Release>:VOXEL_RELEASE>
        $<$<CONFIG:MinSizeRel>:VOXEL_RELEASE>
)

# ============= Third Party Libraries =============
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_hlsl CONFIG REQUIRED)
find_package(spirv_cross_reflect CONFIG REQUIRED)
find_package(spirv_cross_msl CONFIG REQUIRED)
find_package(spirv_cross_cpp CONFIG REQUIRED)
find_package(spirv_cross_c CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
	${Vulkan_LIBRARIES}
	glfw
	tinyobjloader::tinyobjloader
	unofficial::shaderc::shaderc
	spirv-cross-c
    PUBLIC
	spdlog::spdlog
	imgui::imgui
	glm::glm
)