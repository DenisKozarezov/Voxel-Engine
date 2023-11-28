#include "GizmosAPI.h"
#include <vulkan/vkUtils/VulkanGizmos.h>
#include <vulkan/VulkanBackend.h>

namespace utils
{
	GizmosAPI* GizmosAPI::s_instance = nullptr;

	UniqueRef<GizmosAPI> GizmosAPI::Create()
	{
		auto& device = vulkan::getDevice();
		return MakeUnique<vkUtils::VulkanGizmos>(device);
	}

	GizmosAPI::GizmosAPI()
	{
		if (!s_instance)
			s_instance = this;
	}

	void Gizmos::drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color)
	{
		GizmosAPI::s_instance->drawLine(point1, point2, color);
	}
	void Gizmos::drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color)
	{
		GizmosAPI::s_instance->drawWireframeCube(center, size, color);
	}
	void Gizmos::drawWireframeCircle(const glm::vec3& center, const float& radius, const glm::vec3& axis)
	{
		GizmosAPI::s_instance->drawWireframeCircle(center, radius, axis);
	}
}