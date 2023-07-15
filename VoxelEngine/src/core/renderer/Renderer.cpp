#include "Renderer.h"
#include <vulkan/VulkanBackend.h>
#include <imgui.h>
#include "core/Application.h"

namespace VoxelEngine::renderer
{
    RenderPerformanceStats renderPerformanceStats;

    float Renderer::getTime() noexcept
    {
        return (float)glfwGetTime();
    }
    const RenderPerformanceStats& Renderer::getStats()
    {
        renderPerformanceStats.deltaTime = VoxelEngine::Application::getDeltaTime();
        renderPerformanceStats.fps = ImGui::GetIO().Framerate;
        return renderPerformanceStats;
    }
    void Renderer::init(const Window& window, components::camera::FirstPersonCamera* camera)
    {
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