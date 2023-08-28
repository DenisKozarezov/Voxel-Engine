#include "Shader.h"
#include <core/Assert.h>

namespace VoxelEngine::renderer
{
	Shader::Shader(const string& filepath, const ShaderStage& shaderStage) 
		: m_filepath(filepath), m_shaderStage(shaderStage)
	{

	}
	string Shader::readFile(const string& filename)
	{
		std::ifstream file(filename, std::ios::in | std::ios::binary);

		VOXEL_CORE_ASSERT(file.is_open(), "failed to open shader source file at path: " + filename);

		file.seekg(0, std::ios::end);
		size_t size = file.tellg();

		VOXEL_CORE_ASSERT(size >= 0, "failed to read shader source file at path: " + filename);

		string shaderSource;
		shaderSource.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(shaderSource.data(), size);
		return shaderSource;
	}
}
