cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VoxelEngine)

set(PUBLIC_HEADERS src/pch.h)

set_target_properties(${TARGET} PROPERTIES 
    PUBLIC_HEADER ${PUBLIC_HEADERS}
    LINKER_LANGUAGE CXX
)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE CORE_SOURCE ./src/core/*.cpp)
file(GLOB_RECURSE CORE_HEADER ./src/core/*.h)
file(GLOB_RECURSE PLATFORM_SOURCE ./src/platform/*.cpp)
file(GLOB_RECURSE PLATFORM_HEADER ./src/platform/*.h)

source_group("core" FILES ${CORE_SOURCE})
source_group("core" FILES ${CORE_HEADER})
source_group("platform" FILES ${PLATFORM_SOURCE})
source_group("platform" FILES ${PLATFORM_HEADER})

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(${PROJECT_NAME} 
    SHARED 
        ${CORE_SOURCE}
        ${CORE_HEADER}
        ${PLATFORM_SOURCE}
        ${PLATFORM_HEADER}
        ${SOURCE_DIR}/VoxelEngine.h
)

# ============= PREPROCESSOR DEFINES ==============
if (MSVC)
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
        "IMGUI_API= __declspec(dllexport)"
        VOXELENGINE_EXPORTS
        $<$<CONFIG:Debug>:VOXEL_DEBUG>
        $<$<CONFIG:RelWithDebInfo>:VOXEL_DEBUG>
        $<$<CONFIG:Release>:VOXEL_RELEASE>
        $<$<CONFIG:MinSizeRel>:VOXEL_RELEASE>
)

# ============= Third Party Libraries =============
set(EXTERNAL_LIBS_ROOT ${CMAKE_SOURCE_DIR}/ThirdParty)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${Vulkan_INCLUDE_DIR}
    PUBLIC
        ${EXTERNAL_LIBS_ROOT}
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
	    ${Vulkan_LIBRARIES}
	    glfw
    PUBLIC
        glm
        spdlog
        imgui
)