#include "Buffer.h"
#include <vulkan/vkUtils/VulkanVertexBuffer.h>
#include <vulkan/vkUtils/VulkanIndexBuffer.h>
#include <vulkan/VulkanBackend.h>

namespace VoxelEngine::renderer
{
    SharedRef<VertexBuffer> VertexBuffer::Allocate(const uint32& size)
    {
        const auto& physicalDevice = vulkan::getPhysicalDevice();
        const auto& logicalDevice = vulkan::getLogicalDevice();
        return MakeShared<vkUtils::VulkanVertexBuffer>(physicalDevice, logicalDevice, nullptr, size);
    }
    SharedRef<VertexBuffer> VertexBuffer::Allocate(const void* vertices, const uint32& size)
    {
        const auto& physicalDevice = vulkan::getPhysicalDevice();
        const auto& logicalDevice = vulkan::getLogicalDevice();
        return MakeShared<vkUtils::VulkanVertexBuffer>(physicalDevice, logicalDevice, vertices, size);
    }
    SharedRef<IndexBuffer> IndexBuffer::Allocate(const void* indices, const uint32& size)
    {
        const auto& physicalDevice = vulkan::getPhysicalDevice();
        const auto& logicalDevice = vulkan::getLogicalDevice();
        return MakeShared<vkUtils::VulkanIndexBuffer>(physicalDevice, logicalDevice, indices, size);
    }
}