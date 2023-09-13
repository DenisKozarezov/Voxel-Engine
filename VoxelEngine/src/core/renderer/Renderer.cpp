#include <vulkan/VulkanBackend.h>
#include <core/Application.h>

namespace VoxelEngine::renderer
{
    static RenderPerformanceStats s_renderPerformanceStats;

    RenderSettings& Renderer::getRenderSettings()
    {
        return vulkan::getRenderSettings();
    }
    const RenderPerformanceStats& Renderer::getStats()
    {
        auto& app = Application::getInstance();
        s_renderPerformanceStats.deltaTime = app.getDeltaTime();
        s_renderPerformanceStats.fps = app.getFPS();
        s_renderPerformanceStats.frameStats = vulkan::getFrameStats();
        return s_renderPerformanceStats;
    }
    void Renderer::resetStats()
    {
        vulkan::resetFrameStats();
    }
    void Renderer::init(const Window& window)
    {
        VOXEL_CORE_WARN("Renderer initialization.");
                       
        RenderCommand::init(window);
    }
    void Renderer::preRender(const components::camera::Camera& camera)
    {
        UniformBufferObject ubo =
        {
            .view = camera.viewMatrix(),
            .proj = camera.projectionMatrix(),
            .viewproj = ubo.proj * ubo.view,
            .lightPos = camera.getPosition()
        };
        vulkan::beginFrame(ubo);
    }
    void Renderer::postRender()
    {
        vulkan::endFrame();
    }
    void Renderer::resize(const uint32& width, const uint32& height)
    {
        vulkan::resize(width, height);
    }
    void Renderer::submitRenderables(const std::vector<glm::vec3>& renderables)
    {
        vulkan::submitRenderables(renderables);
    }
    void Renderer::deviceWaitIdle()
    {
        vulkan::deviceWaitIdle();
    }
    void Renderer::shutdown()
    {
        vulkan::cleanup();
    }
}