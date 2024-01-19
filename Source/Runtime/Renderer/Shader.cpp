#include "Shader.h"

namespace VoxelEngine::renderer
{
	string Shader::readFile(const string& filename)
	{
		const auto handle = FileManager::createFileReader(filename);
		
		handle->seekg(0, std::ios::end);
		const size_t size = handle->tellg();

		VOXEL_CORE_ASSERT(size >= 0, "failed to read shader source file at path: " + filename);

		string shaderSource;
		shaderSource.resize(size);
		handle->seekg(0, std::ios::beg);
		handle->read(shaderSource.data(), size);
		return shaderSource;
	}
	std::vector<uint32> Shader::readBinary(const string& filename)
	{
		const auto handle = FileManager::createFileReader(filename);
		
		handle->seekg(0, std::ios::end);
		const size_t size = handle->tellg();

		VOXEL_CORE_ASSERT(size >= 0, "failed to read shader binary file at path: " + filename);

		std::vector<uint32> shaderSource;
		shaderSource.resize(size / sizeof(uint32));
		handle->seekg(0, std::ios::beg);
		handle->read((char*)shaderSource.data(), size);
		return shaderSource;
	}

	void Shader::createCachedBinaryFile(const string& cachedPath, const uint32* bytes, const size_t& size)
	{
		const auto handle = FileManager::createFileWriter(cachedPath);
		
		handle->write((char*)bytes, size);
		handle->flush();
	}
}
