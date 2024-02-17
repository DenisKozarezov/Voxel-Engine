cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VModels CXX)

set_target_properties(${TARGET} PROPERTIES 
    LINKER_LANGUAGE CXX
)

file(GLOB_RECURSE CORE_SOURCE *.cpp)
file(GLOB_RECURSE CORE_HEADER *.h)

source_group("core" FILES ${CORE_SOURCE})
source_group("core" FILES ${CORE_HEADER})

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${PROJECT_NAME} 
    STATIC 
        ${CORE_SOURCE}
        ${CORE_HEADER}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER VModels.h)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC        
        VMODELS_EXPORTS
        $<$<CONFIG:Debug>:VMODELS_DEBUG>
        $<$<CONFIG:RelWithDebInfo>:VMODELS_DEBUG>
        $<$<CONFIG:Release>:VMODELS_RELEASE>
        $<$<CONFIG:MinSizeRel>:VMODELS_RELEASE>
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_DIR})

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} 
    PUBLIC
	glm::glm
)