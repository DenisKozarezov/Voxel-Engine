#pragma once
#include <core/renderer/Shader.h>

namespace VoxelEngine::renderer
{
	class VulkanShader : public Shader
	{
	public:
		const string& getName() const override;
		void setUniform(const string& name, const glm::vec2 vector) const override;
		void setUniform(const string& name, const glm::vec3 vector) const override;
		void setUniform(const string& name, const glm::vec4 vector) const override;
		void setUniform(const string& name, const glm::mat4 matrix) const override;
		void setUniform(const string& name, const float value) const override;
	};
}

