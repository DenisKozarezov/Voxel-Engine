#include <vulkan/VulkanRendererAPI.h>
#include <Core/Logging/Assert.h>

namespace VoxelEngine::renderer
{
    static constexpr GraphicsSpec g_graphicsSpec = GraphicsSpec::Vulkan;

    TUniquePtr<RendererAPI> RendererAPI::Create()
    {
        switch (g_graphicsSpec)
        {
        case GraphicsSpec::None:
            VOXEL_CORE_ASSERT(false, "No renderer API selected!");
            break;
        case GraphicsSpec::Vulkan: 
            return MakeUnique<vulkan::VulkanRendererAPI>();
        }
        return nullptr;
    }
}