#include "Renderer.h"
#include <platform/Vulkan/VulkanRenderer.h>

namespace VoxelEngine::renderer
{
    static SharedRef<VulkanRenderer> renderer = 0;

    const float Renderer::getTime()
    {
        return (float)glfwGetTime();
    }
    const void Renderer::init(const SharedRef<Window> window)
    {
        renderer = VulkanRenderer::getInstance();
        renderer->setWindow(window);
        renderer->init();
    }
    const void Renderer::beginFrame()
    {
        renderer->beginFrame();
    }
    const void Renderer::endFrame()
    {
        renderer->endFrame();
    }
    const void Renderer::deviceWaitIdle()
    {
        renderer->deviceWaitIdle();
    }
    const void Renderer::cleanup()
    {
        renderer->cleanup();
    }
}