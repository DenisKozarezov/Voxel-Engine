#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>

enum ShaderStage : byte { Vertex, Fragment, Geometry, Compute };

namespace VoxelEngine::renderer
{
	class Shader
	{
	private:
		string m_name;
	public:
		Shader() noexcept = default;
		Shader(Shader const&) noexcept = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(Shader const& rhs) noexcept = delete;
		Shader& operator=(Shader&& rhs) noexcept = delete;

		inline constexpr string getName() noexcept { return m_name; }
		virtual void unbind() const = 0;

		virtual ~Shader() = default;
	protected:
		string readFile(const string& filename);
	};
}