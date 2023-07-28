#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragNormal;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragNormal = normalize(vec4(inNormal, 1.0)).xyz;
}