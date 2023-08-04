#version 450

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} ubo;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 2) out vec3 nearPoint;
layout(location = 3) out vec3 farPoint;
layout(location = 4) out mat4 fragViewProj;

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 pos = inPosition;
    nearPoint = UnprojectPoint(pos.x, pos.y, 0.0, ubo.view, ubo.proj).xyz;
    farPoint = UnprojectPoint(pos.x, pos.y, 1.0, ubo.view, ubo.proj).xyz;
    fragViewProj = ubo.viewproj;
    gl_Position = vec4(pos, 1.0);
}