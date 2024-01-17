#pragma once
#include <glm/vec3.hpp>
#include <core/Base.h>
#include <vulkan/vkUtils/VulkanMaterials.h>

namespace utils
{
	struct LineVertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};

	struct CircleVertex
	{
		glm::vec3 pos;
		glm::vec3 axis;
		glm::vec3 color;
		float radius;
	};

	struct RenderData
	{
		uint32 linesVertexCount = 0;
		LineVertex* linesPtrStart = nullptr;
		LineVertex* linesPtrCurrent = nullptr;
		SharedRef<VoxelEngine::renderer::VertexBuffer> linesBuffer;
		const vkUtils::VulkanMaterial* linesMaterial = nullptr;

		uint32 circleVertexCount = 0;
		CircleVertex* circlePtrStart = nullptr;
		CircleVertex* circlePtrCurrent = nullptr;
		SharedRef<VoxelEngine::renderer::VertexBuffer> circleBuffer;
		const vkUtils::VulkanMaterial* circleMaterial = nullptr;
	};

	class GizmosAPI
	{
	private:
		RenderData m_renderData;
		static GizmosAPI* s_instance;
	public:
		GizmosAPI();
		~GizmosAPI();

		void startBatch();
		void drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		void drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		void drawWireframeCircle(const glm::vec3& center, const float& radius = 1.0f, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		void onGizmosDraw();
		static GizmosAPI* getInstance();
	};

	class Gizmos
	{
	public:
		INLINE static void drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		INLINE static void drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		INLINE static void drawWireframeCircle(const glm::vec3& center, const float& radius = 1.0f, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
	};
}