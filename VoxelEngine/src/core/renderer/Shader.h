#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

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

		virtual ~Shader() = default;
	protected:
		string readFile(const string& filename);
		std::vector<uint32> readBinary(const string& filename);
	};
}