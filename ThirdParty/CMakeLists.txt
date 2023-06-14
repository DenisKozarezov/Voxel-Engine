# ============ THIRD PARTY LIBRARIES ============

# link GLFW
set(GLFW_LIB_NAME "GLFW")
set(GLFW_INCLUDE_PATH ${GLFW_LIB_NAME}/include)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC  ON  CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_LIB_NAME})

# link spdlog
set(SPDLOG_LIB_NAME "spdlog")
set(SPDLOG_MASTER_PROJECT OFF CACHE BOOL "" FORCE)
add_subdirectory(${SPDLOG_LIB_NAME})

# link glm
set(GLM_LIB_NAME "glm")
set(GLM_INCLUDE_PATH ${GLM_LIB_NAME}/glm)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLM_LIB_NAME})

# link ImGui
find_package(Vulkan REQUIRED)

set(IMGUI_LIB_NAME "imgui")
set(IMGUI_INCLUDE_PATH "${IMGUI_LIB_NAME}/")
set(IMGUI_SOURCES
    "${IMGUI_LIB_NAME}/imgui.cpp"
    "${IMGUI_LIB_NAME}/imgui_demo.cpp"
    "${IMGUI_LIB_NAME}/imgui_draw.cpp"
    "${IMGUI_LIB_NAME}/imgui_tables.cpp"
    "${IMGUI_LIB_NAME}/imgui_widgets.cpp"
    "${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_LIB_NAME}/backends/imgui_impl_vulkan.cpp"
)

set(IMGUI_HEADERS
    "${IMGUI_LIB_NAME}/imconfig.h"
    "${IMGUI_LIB_NAME}/imgui.h"
    "${IMGUI_LIB_NAME}/imgui_internal.h"
    "${IMGUI_LIB_NAME}/imstb_rectpack.h"
    "${IMGUI_LIB_NAME}/imstb_textedit.h"
    "${IMGUI_LIB_NAME}/imstb_truetype.h"
    "${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.h"
    "${IMGUI_LIB_NAME}/backends/imgui_impl_vulkan.h"
)

add_library(${IMGUI_LIB_NAME}
    STATIC
        ${IMGUI_SOURCES}
        ${IMGUI_HEADERS}
)
set(LIBRARY_OUTPUT_DIR "$<0:>${CMAKE_BINARY_DIR}/ThirdParty/${IMGUI_LIB_NAME}/$<CONFIG>")
set_target_properties(${IMGUI_LIB_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR}
	LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIR}
)
target_include_directories(${IMGUI_LIB_NAME}
    PUBLIC
        ${IMGUI_INCLUDE_PATH}
        ${GLFW_INCLUDE_PATH}
        ${Vulkan_INCLUDE_DIR}
)