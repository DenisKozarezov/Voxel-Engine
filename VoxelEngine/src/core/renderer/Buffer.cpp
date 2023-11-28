#include "Buffer.h"
#include <vulkan/vkUtils/VulkanVertexBuffer.h>
#include <vulkan/vkUtils/VulkanIndexBuffer.h>
#include <vulkan/VulkanBackend.h>

namespace VoxelEngine::renderer
{
    SharedRef<VertexBuffer> VertexBuffer::Allocate(const uint32& size)
    {
        return MakeShared<vkUtils::VulkanVertexBuffer>(vulkan::getDevice(), size);
    }
    SharedRef<VertexBuffer> VertexBuffer::Allocate(const void* vertices, const uint32& size)
    {
        return MakeShared<vkUtils::VulkanVertexBuffer>(vulkan::getDevice(), vertices, size);
    }
    SharedRef<IndexBuffer> IndexBuffer::Allocate(const uint32& size)
    {
        return MakeShared<vkUtils::VulkanIndexBuffer>(vulkan::getDevice(), size);
    }
    SharedRef<IndexBuffer> IndexBuffer::Allocate(const void* indices, const uint32& size)
    {
        return MakeShared<vkUtils::VulkanIndexBuffer>(vulkan::getDevice(), indices, size);
    }
}