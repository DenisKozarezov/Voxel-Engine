#pragma once
#include <Core/pch.h>
#include <Core/CoreTypes.h>

enum ShaderStage : byte { None, Vertex, Fragment, Geometry, Compute };

namespace VoxelEngine::renderer
{
	class Shader
	{
	public:
		Shader() noexcept = default;
		Shader(Shader const&) noexcept = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(Shader const& rhs) noexcept = delete;
		Shader& operator=(Shader&& rhs) noexcept = delete;

		virtual void unbind() const = 0;
		static string readFile(const string& filename);
		static std::vector<uint32> readBinary(const string& filename);

		virtual ~Shader() = default;
	};
}