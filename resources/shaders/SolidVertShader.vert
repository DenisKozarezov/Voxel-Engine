#version 450

layout(binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} ubo;

layout (binding = 1) readonly uniform UboInstance 
{
    mat4 model; 
} objectData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;

void main() {
    gl_Position = ubo.viewproj * objectData.model * vec4(inPosition, 1.0);
    fragPos = inPosition;
    fragNormal = normalize(vec4(inNormal, 1.0)).xyz;
}