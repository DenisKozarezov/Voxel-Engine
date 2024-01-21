#pragma once
#include <glm/vec3.hpp>
#include <Renderer/Buffer.h>
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
		TSharedPtr<VoxelEngine::renderer::VertexBuffer> linesBuffer;
		TSharedPtr<const VoxelEngine::renderer::IMaterial> linesMaterial = nullptr;

		uint32 circleVertexCount = 0;
		CircleVertex* circlePtrStart = nullptr;
		CircleVertex* circlePtrCurrent = nullptr;
		TSharedPtr<VoxelEngine::renderer::VertexBuffer> circleBuffer;
		TSharedPtr<const VoxelEngine::renderer::IMaterial> circleMaterial = nullptr;
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
		static void drawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		static void drawWireframeCube(const glm::vec3& center, const glm::vec3& size, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
		static void drawWireframeCircle(const glm::vec3& center, const float& radius = 1.0f, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));
	};
}