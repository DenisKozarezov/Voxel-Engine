#include "GizmosAPI.h"
#include <vulkan/vkUtils/VulkanGizmos.h>
#include <vulkan/VulkanBackend.h>

namespace utils
{
	GizmosAPI* GizmosAPI::s_instance = nullptr;

	UniqueRef<GizmosAPI> GizmosAPI::Create()
	{
		auto& physicalDevice = vulkan::getPhysicalDevice();
		auto& logicalDevice = vulkan::getLogicalDevice();
		return MakeUnique<vkUtils::VulkanGizmos>(physicalDevice, logicalDevice);
	}

	GizmosAPI::GizmosAPI()
	{
		if (!s_instance)
			s_instance = this;
	}

	void Gizmos::drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color)
	{
		GizmosAPI::getInstance()->drawLine(point1, point2, color);
	}
	void Gizmos::drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color)
	{
		GizmosAPI::getInstance()->drawWireframeCube(center, size, color);
	}
	void Gizmos::drawWireframeCircle(const glm::vec3& center, const float& radius, const glm::vec3& axis)
	{
		GizmosAPI::getInstance()->drawWireframeCircle(center, radius, axis);
	}
}