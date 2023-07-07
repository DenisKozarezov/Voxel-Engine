#include "Renderer.h"
#include <platform/Vulkan/VulkanRenderer.h>

namespace VoxelEngine::renderer
{
    static SharedRef<VulkanRenderer> renderer = 0;

    float Renderer::getTime() noexcept
    {
        return (float)glfwGetTime();
    }
    void Renderer::init(const Window& window)
    {
        renderer = VulkanRenderer::getInstance();
        renderer->setWindow(window);
        renderer->init();
    }
    void Renderer::beginFrame()
    {
        renderer->beginFrame();
    }
    void Renderer::endFrame()
    {
        renderer->endFrame();
    }
    void Renderer::deviceWaitIdle()
    {
        renderer->deviceWaitIdle();
    }
    void Renderer::cleanup()
    {
        renderer->cleanup();
    }
}