#type vertex
#version 450

layout(binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inInstancePosition;

layout(location = 0) out vec3 fragColor;

void main() {
    vec4 pos = vec4(inPosition + inInstancePosition, 1.0);
    gl_Position = ubo.viewproj * pos;
}

#type fragment
#version 450

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}