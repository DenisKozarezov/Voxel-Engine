#include "Shader.h"

namespace VoxelEngine::renderer
{
	Shader::Shader(const string& filepath) : _filepath(filepath)
	{

	}
	std::vector<char> Shader::readFile(const string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file! Path: " + filename);
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
}
