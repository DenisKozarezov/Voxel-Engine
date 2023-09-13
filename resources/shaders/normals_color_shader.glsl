#type vertex
#version 450

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec3 lightPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inInstancePosition;

layout(location = 0) out vec3 outNormal;

void main() {
    gl_Position = ubo.viewproj * vec4(inPosition + inInstancePosition, 1.0);
    outNormal = inNormal;
}

#type fragment
#version 450

layout(location = 0) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inNormal, 1.0);
}