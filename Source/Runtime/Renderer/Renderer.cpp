#include <Vulkan/VulkanBackend.h>
#include <Core/Internal/Application.h>
#include "RenderingStructs.h"
#include "GizmosAPI.h"
#include "RenderCommand.h"

namespace VoxelEngine::renderer
{
    RenderPerformanceStats Renderer::s_renderPerformanceStats{};
    RenderSettings Renderer::s_renderSettings{};
    utils::GizmosAPI* Renderer::s_gizmosAPI = nullptr;
    
    void Renderer::resetStats()
    {
        s_renderPerformanceStats.frameStats.drawCalls = 0;
        s_renderPerformanceStats.frameStats.triangles = 0;
        s_renderPerformanceStats.frameStats.vertices = 0;
        s_renderPerformanceStats.frameStats.primitives = 0;
        s_renderPerformanceStats.batches = 0;
        s_renderPerformanceStats.frameStats.clippingPrimitives = 0;
        s_renderPerformanceStats.frameStats.vertexShaderInvocations = 0;
        s_renderPerformanceStats.frameStats.fragmentShaderInvocations = 0;
    }

    void Renderer::flushStats()
    {
        const auto* app = Application::getInstance();
        s_renderPerformanceStats.deltaTime = app->getDeltaTime();
        s_renderPerformanceStats.fps = app->getFPS();

        const uint32 drawCalls = s_renderPerformanceStats.frameStats.drawCalls;
        s_renderPerformanceStats.frameStats = vulkan::getFrameStats();
        s_renderPerformanceStats.frameStats.drawCalls = drawCalls;
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
    void Renderer::resize(const uint16& width, const uint16& height)
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
