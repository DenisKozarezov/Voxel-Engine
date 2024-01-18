#pragma once
#include <filesystem>
#include <Engine/Components/mesh/Mesh.h>

#if defined(VOXEL_PLATFORM_WINDOWS)
#define PATH_SEPARATOR "\\"
#else 
#define PATH_SEPARATOR "/"
#endif

const string g_environmentPathCache = std::filesystem::current_path().string() + PATH_SEPARATOR;

#define RESOURCES_FOLDER_NAME "Resources"
#define RESOURCES_DIR RESOURCES_FOLDER_NAME PATH_SEPARATOR
#define ASSET_PATH(path) RESOURCES_DIR path
#define ASSET_ABSOLUTE_PATH(relativePath) g_environmentPathCache + ASSET_PATH(relativePath)
#define CACHE_PATH(path) ASSET_PATH("cache" PATH_SEPARATOR ##path)
#define SHADERS_PATH(path) ASSET_PATH("shaders" PATH_SEPARATOR##path)

using Mesh = VoxelEngine::components::mesh::Mesh;

namespace assets
{
	enum ImageColorFormat : byte
	{
		Default = 0, // only used for desired_channels
		Grey = 1,
		Grey_alpha = 2,
		RGB = 3,
		RGB_alpha = 4
	};

	struct TextureData
	{
	public:
		byte* m_nativePtr = nullptr;
		int m_width;
		int m_height;
		int m_texChannels;

		TextureData() noexcept : m_width(0), m_height(0), m_texChannels(4) { }
		~TextureData() = default;

		FORCE_INLINE const bool& isValid() const { return m_nativePtr; }
		
		void release() const;
	};

	static class AssetsProvider
	{
	public:
		static const TextureData loadTexture(const string& path, ImageColorFormat req_comp = ImageColorFormat::RGB_alpha);
		static const SharedRef<Mesh> loadObjMesh(const string& path);
	};
}