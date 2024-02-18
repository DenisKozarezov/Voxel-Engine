#pragma once
#include <Core/HAL/AssetsManager/Paths.h>
#include <Core/HAL/AssetsManager/FileManager.h>
#include <Core/Logging/Assert.h>

enum class ShaderStage : byte { None, Vertex, Fragment, Geometry, Compute };

constexpr ShaderStage shaderStageFromString(const string& type)
{
	if (type == "vertex")
		return ShaderStage::Vertex;
	if (type == "fragment")
		return ShaderStage::Fragment;
	if (type == "geometry")
		return ShaderStage::Geometry;
	if (type == "compute")
		return ShaderStage::Compute;

	RUNTIME_ASSERT(0, "Unsupported shader stage passed to function.");
	return ShaderStage::None;
}

constexpr string shaderStageString(const ShaderStage& stage)
{
	switch (stage)
	{
#define STR(x) case ShaderStage::##x: return #x
		STR(Vertex);
		STR(Fragment);
		STR(Geometry);
		STR(Compute);
#undef STR		
	}
	RUNTIME_ASSERT(0, "Unsupported shader stage passed to function.");
	return "";
}

static string getShaderCacheDirectory()
{
	return Paths::cacheDir() + "shaders/SPIR-V";
}

static void createShaderCacheDirectoryIfNeeded()
{
	const string cacheDirectory = getShaderCacheDirectory();
	if (!Paths::directoryExists(cacheDirectory))
	{
		RUNTIME_WARN("Shaders cache directory doesn't exists. Creating new folder...");
		FileManager::makeDirectory(cacheDirectory);
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
		
		static void createCachedBinaryFile(const string& cachedPath, const uint32* bytes, const size_t& size);

		virtual ~Shader() = default;
	};
}