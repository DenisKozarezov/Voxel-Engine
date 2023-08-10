#include <vulkan/VulkanBackend.h>
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
        auto& app = Application::getInstance();
        renderPerformanceStats.deltaTime = app.getDeltaTime();
        renderPerformanceStats.fps = app.getFPS();
        renderPerformanceStats.frameStats = vulkan::getFrameStats();
        return renderPerformanceStats;
    }
    void Renderer::init(const Window& window)
    {
        VOXEL_CORE_WARN("Renderer initialization.");
        vulkan::setWindow(window);
        vulkan::init();
    }
    void Renderer::preRender()
    {
        vulkan::beginFrame();
    }
    void Renderer::render()
    {
        vulkan::resetFrameStats();
    }
    void Renderer::postRender()
    {
        vulkan::endFrame();
    }
    void Renderer::updateUIOverlay()
    {
        vulkan::updateUIOverlay();
    }
    void Renderer::resize(const uint32& width, const uint32& height)
    {
        vulkan::resize(width, height);
    }
    void Renderer::setCamera(const components::camera::Camera& camera)
    {
        vulkan::setCamera(camera);
    }
    void Renderer::submitRenderables(const std::vector<glm::vec3> objects)
    {
        vulkan::submitRenderables(objects);
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