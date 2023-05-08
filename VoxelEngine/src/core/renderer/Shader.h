#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	class Shader
	{
	public:
		Shader() = delete;

		virtual const string& getName() const = 0;
		virtual void setUniform(const string& name, const glm::vec2 vector) const = 0;
		virtual void setUniform(const string& name, const glm::vec3 vector) const = 0;
		virtual void setUniform(const string& name, const glm::vec4 vector) const = 0;
		virtual void setUniform(const string& name, const glm::mat4 matrix) const = 0;
		virtual void setUniform(const string& name, const float value) const = 0;

		virtual ~Shader() = default;
	};
}