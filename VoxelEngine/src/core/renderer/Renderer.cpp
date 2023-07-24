#include "Renderer.h"
#include <vulkan/VulkanBackend.h>
#include <imgui.h>
#include "core/Application.h"

namespace VoxelEngine::renderer
{
    RenderPerformanceStats renderPerformanceStats;

    const RenderPerformanceStats& Renderer::getStats()
    {
        renderPerformanceStats.deltaTime = Application::getInstance().getDeltaTime();
        renderPerformanceStats.fps = ImGui::GetIO().Framerate;
        return renderPerformanceStats;
    }
    const float Renderer::getTime()
    {
        return (float)glfwGetTime();
    }
    void Renderer::init(const Window& window, components::camera::Camera* camera)
    {
        VOXEL_CORE_WARN("Renderer initialization.")
        vulkan::setWindow(window);
        vulkan::setCamera(camera);
        vulkan::init();
    }
    void Renderer::beginFrame()
    {
        vulkan::beginFrame();
    }
    void Renderer::endFrame()
    {
        vulkan::endFrame();
    }
    void Renderer::deviceWaitIdle()
    {
        vulkan::deviceWaitIdle();
    }
    void Renderer::cleanup()
    {
        vulkan::cleanup();
    }
}