#pragma once
#include <vulkan/vkInit/VulkanDevice.h>
#include <assets_management/AssetsProvider.h>

namespace vkUtils
{
#define MATERIALS_MAX 20

	struct VulkanPipelineCreateInfo
	{
		VkPipelineCreateFlags flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
		VkPipelineVertexInputStateCreateInfo* vertexInputInfo = nullptr;
		VkPipelineInputAssemblyStateCreateInfo* inputAssembly = nullptr;
		VkPipelineViewportStateCreateInfo* viewportState = nullptr;
		VkPipelineRasterizationStateCreateInfo* rasterizer = nullptr;
		VkPipelineMultisampleStateCreateInfo* multisampling = nullptr;
		VkPipelineColorBlendAttachmentState* colorBlendAttachment = nullptr;
		VkPipelineColorBlendStateCreateInfo* colorBlending = nullptr;
		VkPipelineDepthStencilStateCreateInfo* depthStencil = nullptr;
		VkPipelineDynamicStateCreateInfo* dynamicState = nullptr;
		VkDescriptorSetLayout* descriptorSetLayout = nullptr;
		VkRenderPass* renderPass = nullptr;
		VkPipelineShaderStageCreateInfo* shaderStages = nullptr;
		VkPipeline basePipelineHandle = VK_NULL_HANDLE;
		VkPipelineLayoutCreateInfo pipelineLayoutInfo;
		uint32 stagesCount;

		void build(
			const VkDevice& logicalDevice,
			const VkPipelineLayout& pipelineLayout,
			const VkPipelineCache& pipelineCache,
			VkPipeline* pipeline);
	};

	class VulkanMaterial : public VoxelEngine::components::mesh::IMaterial
	{
	public:
		VkDevice logicalDevice = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

		VulkanMaterial(const bool& instanced = false) : IMaterial(instanced) { }
		~VulkanMaterial() override
		{
			vkDestroyPipeline(logicalDevice, pipeline, nullptr);
			vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
		}

		void bind() const override;
		void bind(const VkCommandBuffer& commandBuffer, const VkDescriptorSet& descriptorSet) const;
	};

	using MaterialsCache = std::unordered_map<string, VulkanMaterial*>;
	
	const VulkanMaterial* createMaterial(
		const VkDevice& logicalDevice,
		const VkPipeline& matPipeline,
		const VkPipelineLayout& matLayout,
		const string& matName,
		const bool& instanced = false);
	
	void unregisterMaterial(const string& matName);
	
	const VulkanMaterial* getMaterial(const string& matName);

	void makeMaterials(
		const vkInit::VulkanDevice& device,
		const VkPipelineCache& pipelineCache,
		VulkanPipelineCreateInfo& pipelineInfo);
	
	void releaseMaterials();
}

namespace utils
{
	const VoxelEngine::components::mesh::IMaterial* getMaterial(const string& matName);
}