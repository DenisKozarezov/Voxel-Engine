#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inInstancePosition;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragColor;

void main() {
    vec4 pos = vec4(inPosition + inInstancePosition, 1.0);
    gl_Position = pos;
    fragNormal = normalize(vec4(inNormal, 1.0)).xyz;
    fragColor = inColor;
}