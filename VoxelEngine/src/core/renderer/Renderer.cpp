#include "Renderer.h"
#include <vulkan/VulkanBackend.h>

namespace VoxelEngine::renderer
{
    float Renderer::getTime() noexcept
    {
        return (float)glfwGetTime();
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