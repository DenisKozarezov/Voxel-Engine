#version 450

layout (location = 0) in vec4 inPos;
layout (location = 2) in vec3 inColor;

layout (binding = 0) uniform UniformBufferObject 
{
	mat4 view;
	mat4 proj;
	mat4 viewproj;
} ubo;

layout (location = 0) out vec3 outColor;

void main() 
{
	outColor = inColor;
	gl_Position = ubo.viewproj * inPos;
}
