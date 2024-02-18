#pragma once
#include <filesystem>
#include <Engine/Components/mesh/Mesh.h>

#if defined(VOXEL_PLATFORM_WINDOWS)
#define PATH_SEPARATOR "\\"
#else 
#define PATH_SEPARATOR "/"
#endif

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

	class AssetsProvider
	{
	public:
		static const TextureData loadTexture(const string& path, ImageColorFormat req_comp = ImageColorFormat::RGB_alpha);
		static const TSharedPtr<Mesh> loadObjMesh(const string& path);
	};
}