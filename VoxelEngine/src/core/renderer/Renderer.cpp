#include "Renderer.h"
#include <vulkan/VulkanBackend.h>
#include <imgui.h>
#include "core/Application.h"

namespace VoxelEngine::renderer
{
    RenderPerformanceStats renderPerformanceStats;

    RenderSettings& Renderer::getRenderSettings()
    {
        return vulkan::getRenderSettings();
    }
    const RenderPerformanceStats& Renderer::getStats()
    {
        renderPerformanceStats.deltaTime = Application::getInstance().getDeltaTime();
        renderPerformanceStats.fps = ImGui::GetIO().Framerate;

        const auto& stats = vulkan::getPipelineStats();

        renderPerformanceStats.pipelineStatNames = stats._Myfirst._Val;
        renderPerformanceStats.pipelineStats = stats._Get_rest()._Myfirst._Val;
        return renderPerformanceStats;
    }
    const float Renderer::getTime()
    {
        return (float)glfwGetTime();
    }
    void Renderer::init(const Window& window)
    {
        VOXEL_CORE_WARN("Renderer initialization.")
        vulkan::setWindow(window);
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
    void Renderer::setCamera(const components::camera::Camera* camera)
    {
        vulkan::setCamera(camera);
    }
    void Renderer::setScene(const VoxelEngine::Scene* scene)
    {
        vulkan::setScene(scene);
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