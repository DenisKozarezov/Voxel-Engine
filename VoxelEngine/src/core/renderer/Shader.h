#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	class Shader
	{
	private:
		string _name;
		string _filepath;
	public:
		Shader() = delete;
		Shader(const string& filepath);
		Shader(Shader const&) noexcept = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator= (Shader const& rhs) noexcept = delete;
		Shader& operator= (Shader&& rhs) noexcept = delete;

		inline const string& getName() const & noexcept { return _name; }
		virtual void setUniform(const string& name, const glm::vec2 vector) const = 0;
		virtual void setUniform(const string& name, const glm::vec3 vector) const = 0;
		virtual void setUniform(const string& name, const glm::vec4 vector) const = 0;
		virtual void setUniform(const string& name, const glm::mat4 matrix) const = 0;
		virtual void setUniform(const string& name, const float value) const = 0;
		virtual void unbind() const = 0;

		virtual ~Shader() = default;
	protected:
		std::vector<char> readFile(const string& filename);
	};
}