#include <vulkan/VulkanBackend.h>
#include <core/Application.h>

namespace VoxelEngine::renderer
{
    static RenderPerformanceStats s_renderPerformanceStats;
    UniqueRef<utils::GizmosAPI> Renderer::s_gizmosAPI = nullptr;

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

        s_gizmosAPI = utils::GizmosAPI::Create();
    }
    void Renderer::preRender(const components::camera::Camera& camera)
    {
        UniformBufferObject ubo =
        {
            .view = camera.viewMatrix(),
            .proj = camera.projectionMatrix(),
            .lightPos = glm::vec3(0.0f, 20.0f, 0.0f)
        };

        s_gizmosAPI->startBatch();
        vulkan::beginFrame(ubo);
    }
    void Renderer::render()
    {
        s_gizmosAPI->onGizmosDraw();
    }
    void Renderer::postRender()
    {
        vulkan::endFrame();
    }
    void Renderer::resize(const uint32& width, const uint32& height)
    {
        vulkan::resize(width, height);
    }
    void Renderer::deviceWaitIdle()
    {
        vulkan::deviceWaitIdle();
    }
    void Renderer::shutdown()
    {
        s_gizmosAPI.reset();
        vulkan::cleanup();
    }
}