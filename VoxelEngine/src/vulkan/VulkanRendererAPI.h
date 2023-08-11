#pragma once
#include <core/renderer/RendererAPI.h>

namespace vulkan
{
	class VulkanRendererAPI : public VoxelEngine::renderer::RendererAPI
	{
	public:
		VulkanRendererAPI() noexcept = default;

		void init() override;
		void setViewport(const uint32& x, const uint32& y, const uint32& width, const uint32& height) override;
		void setClearColor(const glm::vec4& color) override;
		void setLineWidth(const float& width) override;

		~VulkanRendererAPI() = default;
	};
}