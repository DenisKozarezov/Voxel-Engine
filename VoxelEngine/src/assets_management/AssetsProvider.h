#pragma once
#include <core/Base.h>
#include <components/mesh/Mesh.h>
#include <filesystem>

#if defined(VOXEL_PLATFORM_WINDOWS)
#define PATH_SEPARATOR "\\"
#else 
#define PATH_SEPARATOR "/"
#endif

#define RESOURCES_FOLDER_NAME "resources"
#define RESOURCES_DIR RESOURCES_FOLDER_NAME PATH_SEPARATOR
#define ASSET_PATH(path) RESOURCES_DIR path
#define ASSET_ABSOLUTE_PATH(relativePath) std::filesystem::absolute(std::filesystem::path(ASSET_PATH(relativePath))).generic_string()

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
		byte* m_nativePtr;
		int m_width;
		int m_height;
		int m_texChannels;

		TextureData() noexcept = default;
		~TextureData() noexcept = default;

		inline constexpr bool isValid() const { return m_nativePtr; }
		
		void release() const;
	};

	static class AssetsProvider
	{
	public:
		static const TextureData loadTexture(const string& path, ImageColorFormat req_comp = ImageColorFormat::RGB_alpha);
		static const VoxelEngine::SharedRef<Mesh> loadObjMesh(const string& path);
	};
}