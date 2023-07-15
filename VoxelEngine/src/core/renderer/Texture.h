#pragma once
#include <pch.h>
#include <core/PrimitiveTypes.h>
#include <core/renderer/Buffers.h>

namespace VoxelEngine::renderer
{
	class Texture
	{
	protected:
		const std::vector<vulkan::Vertex> _vertices = 
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		};

		const std::vector<uint16> _indices = { 0,1,2,2,3,0 };

		VertexBuffer* _vertexBuffer;
		IndexBuffer* _indexBuffer;
		std::vector<UniformBuffer*> _uniformBuffers;

		virtual void createTextureImage(const std::string& path) = 0;
		virtual void createTextureImageView() = 0;
		virtual void createTextureSampler() = 0;
		virtual void generateQuad() = 0;
	protected:
		uint32 _width = 0;
		uint32 _height = 0;
		uint16 _texChannels = 0;
		string _filepath;
	public:
		Texture(const std::string& path);
		Texture(Texture const&) noexcept = delete;
		Texture(Texture&&) noexcept = delete;
		Texture& operator= (Texture const& rhs) noexcept = delete;
		Texture& operator= (Texture&& rhs) noexcept = delete;

		inline const uint32& getWidth() const& noexcept { return _width; }
		inline const uint32& getHeight() const& noexcept { return _height; }
		inline const int& textureChannelsCount() const& noexcept { return _texChannels; }
		virtual void render();
		virtual void release();
		virtual void setUniformBuffer(const void* ubo, const size_t& size) = 0;

		static Texture* Create(const std::string& path);
	};
}