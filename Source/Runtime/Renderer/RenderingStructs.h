#pragma once
#include <glm/mat4x4.hpp>

struct InstanceData
{
	glm::vec3 pos;
};

struct UniformBufferObject
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	glm::vec3 lightPos;
};
