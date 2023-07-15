#include "Texture.h"
#include "vulkan/VulkanTexture.h"
#include <core/Log.h>

namespace VoxelEngine::renderer
{
	Texture::Texture(const std::string& path) : _filepath(path)
	{
		_vertexBuffer = VertexBuffer::Create(_vertices.data(), sizeof(_vertices[0]) * _vertices.size());
		_indexBuffer = IndexBuffer::Create(_indices.data(), sizeof(_indices[0]) * _indices.size());

		_uniformBuffers.resize(2);
		for (size_t i = 0; i < 2; i++)
		{
			_uniformBuffers[i] = UniformBuffer::Create(sizeof(UniformBufferObject));
		}
	}

	void Texture::render()
	{
		_vertexBuffer->bind();
		_indexBuffer->bind();
	}

	void Texture::release()
	{
		VOXEL_CORE_TRACE("Clearing texture '{0}' [W: {1}; H: {2}; Channels: {3}]...", _filepath, _width, _height, _texChannels);

		_vertexBuffer->release();
		_indexBuffer->release();
		for (int i = 0; i < _uniformBuffers.size(); ++i)
		{
			_uniformBuffers[i]->release();
		}
	}

	Texture* Texture::Create(const std::string& path)
	{
		return nullptr;
	}
}