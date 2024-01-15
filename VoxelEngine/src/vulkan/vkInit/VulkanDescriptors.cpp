#include "VulkanDescriptors.h"
#include "../vkUtils/VulkanValidation.h"
#include "VulkanInitializers.h"
#include <imgui_internal.h>
#include "VulkanDevice.h"

namespace vkInit
{
    VkDescriptorSetLayout createDescriptorSetLayout(const VulkanDevice& device, const DescriptorSetLayoutInputBundle& bindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(bindings.count);

        for (int i = 0; i < bindings.count; ++i)
        {
            VkDescriptorSetLayoutBinding layoutBinding = vkInit::descriptorSetLayoutBinding(
                bindings.types[i],
                bindings.stages[i],
                bindings.indices[i],
                bindings.counts[i]
            );
            layoutBindings.push_back(layoutBinding);
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo = descriptorSetLayoutCreateInfo(layoutBindings);

        VkDescriptorSetLayout layout;
        VkResult err = vkCreateDescriptorSetLayout(device.logicalDevice, &layoutInfo, nullptr, &layout);
        VK_CHECK(err, "failed to create descriptor set layout!");

        VOXEL_CORE_TRACE("Vulkan descriptor set layout created.")

        return layout;
    }

    VkDescriptorPool createDescriptorPool(const VulkanDevice& device)
    {
        const auto& pool_sizes = descriptorPoolSize();

        uint32 size = static_cast<uint32>(pool_sizes.size());
        VkDescriptorPoolCreateInfo poolInfo = descriptorPoolCreateInfo(
            pool_sizes.data(),
            size,
            1000 * IM_ARRAYSIZE(pool_sizes.data()));

        VkDescriptorPool pool;
        VkResult err = vkCreateDescriptorPool(device.logicalDevice, &poolInfo, nullptr, &pool);
        VK_CHECK(err, "failed to create descriptor pool!");

        VOXEL_CORE_TRACE("Vulkan descriptor pool created.")

        return pool;
    }

    VkDescriptorSet allocateDescriptorSet(
        const VulkanDevice& device,
        const VkDescriptorPool& descriptorPool,
        const VkDescriptorSetLayout& descriptorSetLayout)
    {
        VkDescriptorSetAllocateInfo allocInfo = descriptorSetAllocateInfo(
            descriptorPool,
            &descriptorSetLayout
        );

        VkDescriptorSet descriptorSet;
        VkResult err = vkAllocateDescriptorSets(device.logicalDevice, &allocInfo, &descriptorSet);
        VK_CHECK(err, "failed to allocate descriptor set!");

        VOXEL_CORE_TRACE("Vulkan descriptor set allocated.")

        return descriptorSet;
    }
}