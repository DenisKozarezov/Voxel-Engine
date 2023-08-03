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
layout(location = 3) in vec3 inInstancePosition;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColor;
layout(location = 3) out vec3 outLightPos;
layout(location = 4) out vec3 outViewPos;

void main() {
    vec4 pos = vec4(inPosition + inInstancePosition, 1.0);

    gl_Position = ubo.viewproj * pos;

    outPosition = inPosition;
    outNormal = inNormal;
    outColor = inColor;
    outLightPos = ubo.lightPos;
    outViewPos = -pos.xyz;
}