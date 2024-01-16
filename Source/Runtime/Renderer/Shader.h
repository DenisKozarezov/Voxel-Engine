#pragma once
#include <Core/pch.h>
#include <Core/CoreTypes.h>

enum ShaderStage : byte { None, Vertex, Fragment, Geometry, Compute };

constexpr ShaderStage shaderStageFromString(const string& type)
{
	if (type == "vertex")
		return Vertex;
	if (type == "fragment")
		return Fragment;
	if (type == "geometry")
		return Geometry;
	if (type == "compute")
		return Compute;
		
	return None;
}

constexpr string shaderStageString(const ShaderStage& stage)
{
	switch (stage)
	{
#define STR(x) case ShaderStage::##x: return #x;
		STR(Vertex);
		STR(Fragment);
		STR(Geometry);
		STR(Compute);
#undef STR
	}
}

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
		
		static string readFile(const string& filename);
		static std::vector<uint32> readBinary(const string& filename);

		virtual ~Shader() = default;
	};
}