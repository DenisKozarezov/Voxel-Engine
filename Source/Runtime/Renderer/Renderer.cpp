#include <Vulkan/VulkanBackend.h>
#include <Core/Internal/Application.h>
#include "UniformBuffer.h"
#include "GizmosAPI.h"
#include "RenderCommand.h"

namespace VoxelEngine::renderer
{
    RenderPerformanceStats Renderer::s_renderPerformanceStats;
    utils::GizmosAPI* Renderer::s_gizmosAPI = nullptr;

    RenderSettings& Renderer::getRenderSettings()
    {
        return vulkan::getRenderSettings();
    }
    const RenderPerformanceStats& Renderer::getStats()
    {
        return s_renderPerformanceStats;
    }
    void Renderer::resetStats()
    {
        vulkan::resetFrameStats();
    }

    void Renderer::flushStats()
    {
        auto* app = Application::getInstance();
        s_renderPerformanceStats.deltaTime = app->getDeltaTime();
        s_renderPerformanceStats.fps = app->getFPS();
        s_renderPerformanceStats.frameStats = vulkan::getFrameStats();
        s_renderPerformanceStats.shaderStats = vulkan::getShaderStats();
    }

    void Renderer::init(const Window& window)
    {
        RUNTIME_WARN("Renderer initialization.");
                       
        RenderCommand::init(window);

        s_gizmosAPI = utils::GizmosAPI::getInstance();
    }
    void Renderer::preRender(const components::camera::Camera& camera)
    {
        const UniformBufferObject ubo =
        {
            .view = camera.viewMatrix(),
            .proj = camera.projectionMatrix(),
            .lightPos = glm::vec3(0.0f, 20.0f, 0.0f)
        };

        s_gizmosAPI->startBatch();
        vulkan::beginFrame(ubo);
    }

    void Renderer::preRender()
    {
        constexpr UniformBufferObject ubo =
        {
            .view = glm::mat4(0.0f),
            .proj = glm::mat4(0.0f),
            .lightPos = glm::vec3(0.0f, 20.0f, 0.0f)
        };
        
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
        delete s_gizmosAPI;
        vulkan::cleanup();
    }
}
