#pragma once
#include <Renderer/Shader.h>
#include <Renderer/RendererAPI.h>
#include <Engine/Components/camera/Camera.h>
#include <spdlog/spdlog.h>

#pragma region GRAPHICS
constexpr const char* graphicsSpecString(const VoxelEngine::renderer::GraphicsSpec& spec)
{
	switch (spec)
	{
#define STR(x) case VoxelEngine::renderer::GraphicsSpec::##x: return #x;
		STR(None);
		STR(Vulkan);
		STR(OpenGL);
		STR(DirectX12);
#undef STR
	default: return "UNKNOWN_RENDERER_SPEC";
	}
}

constexpr const char* projectionTypeString(const VoxelEngine::components::camera::ProjectionType& mode)
{
	switch (mode)
	{
#define STR(x) case VoxelEngine::components::camera::ProjectionType::##x: return #x;
		STR(Perspective);
		STR(Orthographic);
#undef STR
	}
}

constexpr const char* shaderStageString(const ShaderStage& stage)
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

	VOXEL_CORE_ASSERT(false, "Unknown shader stage!");
	return None;
}
#pragma endregion

constexpr const char* logLevelString(const spdlog::level::level_enum& level)
{
	switch (level)
	{
#define STR(x) case spdlog::level::##x: return #x;
		STR(info);
		STR(warn);
		STR(critical);
		STR(err);
#undef STR
	default: return "info";
	}
}