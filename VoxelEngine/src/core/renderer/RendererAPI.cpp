#include <vulkan/VulkanRendererAPI.h>
#include <core/Log.h>

namespace VoxelEngine::renderer
{
    const GraphicsSpec g_graphicsSpec = GraphicsSpec::Vulkan;

    UniqueRef<RendererAPI> RendererAPI::Create()
    {
        switch (g_graphicsSpec)
        {
        case GraphicsSpec::None:
            VOXEL_CORE_CRITICAL("Unknown renderer API!");
            break;
        case GraphicsSpec::Vulkan: return UniqueRef<RendererAPI>(new vulkan::VulkanRendererAPI());
        }
    }
}