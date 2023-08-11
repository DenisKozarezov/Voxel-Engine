#include <vulkan/VulkanRendererAPI.h>
#include <core/Log.h>

namespace VoxelEngine::renderer
{
    const renderer::RendererAPI::API g_rendererAPI = renderer::RendererAPI::API::Vulkan;

    UniqueRef<RendererAPI> RendererAPI::Create()
    {
        switch (g_rendererAPI)
        {
        case API::None:
            VOXEL_CORE_CRITICAL("No renderer API!");
            break;
        case API::Vulkan: return UniqueRef<RendererAPI>(new vulkan::VulkanRendererAPI());
        }
    }
}