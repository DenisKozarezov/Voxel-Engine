#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} ubo;

layout(std140, binding = 1) readonly buffer storageBuffer {
    mat4 model[];
} objectData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;

void main() {
    gl_Position = ubo.viewproj * objectData.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
    fragPos = inPosition;
    fragNormal = normalize(vec4(inNormal, 1.0)).xyz;
}