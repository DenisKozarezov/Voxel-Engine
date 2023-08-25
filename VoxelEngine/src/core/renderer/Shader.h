#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::renderer
{
	class Shader
	{
	private:
		string m_name;
		string m_filepath;
	public:
		Shader() = delete;
		Shader(const string& filepath);
		Shader(Shader const&) noexcept = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(Shader const& rhs) noexcept = delete;
		Shader& operator=(Shader&& rhs) noexcept = delete;

		inline const string& getName() const noexcept { return m_name; }
		virtual void unbind() const = 0;

		virtual ~Shader() = default;
	protected:
		std::vector<char> readFile(const string& filename);
	};
}