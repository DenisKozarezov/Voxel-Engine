#include "Texture.h"
#include "vulkan/VulkanTexture.h"
#include <core/Log.h>

namespace VoxelEngine::renderer
{
	void Texture::generateQuad()
	{
		_vertexBuffer = VertexBuffer::Create(_vertices.data(), sizeof(_vertices[0]) * _vertices.size());
		_indexBuffer = IndexBuffer::Create(_indices.data(), sizeof(_indices[0]) * _indices.size());

		_uniformBuffers.resize(2);
		for (size_t i = 0; i < 2; i++)
		{
			_uniformBuffers[i] = UniformBuffer::Create(sizeof(UniformBufferObject));
		}
	}

	Texture::Texture(const std::string& path) : _filepath(path)
	{
		
	}

	void Texture::render()
	{
		_vertexBuffer->bind();
		_indexBuffer->bind();
	}

	void Texture::release()
	{
		std::stringstream ss;
		ss << "Clearing texture '" << _filepath << "' [W: " << _width << "; H: " << _height << "; Channels: " << _texChannels << "]...";
		VOXEL_CORE_TRACE(ss.str())

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