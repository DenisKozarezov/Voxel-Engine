#include "Shader.h"
#include <core/Assert.h>

namespace VoxelEngine::renderer
{
	Shader::Shader(const string& filepath) : m_filepath(filepath)
	{

	}
	std::vector<char> Shader::readFile(const string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		VOXEL_CORE_ASSERT(file.is_open(), "failed to open file! Path: " + filename)

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
}
