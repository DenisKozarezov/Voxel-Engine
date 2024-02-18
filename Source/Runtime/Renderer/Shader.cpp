#include "Shader.h"

namespace VoxelEngine::renderer
{
	void Shader::createCachedBinaryFile(const string& cachedPath, const uint32* bytes, const size_t& size)
	{
		const auto handle = FileManager::createFileWriter(cachedPath);
		handle->write((char*)bytes, size);
		handle->flush();
	}
}
