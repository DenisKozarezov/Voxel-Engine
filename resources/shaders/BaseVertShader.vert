#version 450

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec3 lightPos;
} ubo;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragColor;

void main() {
    gl_Position = ubo.viewproj * vec4(inPosition, 1.0);
    fragPos = inPosition;
    fragNormal = inNormal;
    fragColor = inColor;
}