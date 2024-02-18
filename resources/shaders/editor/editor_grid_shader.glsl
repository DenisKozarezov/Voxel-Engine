#type vertex
#version 450

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;

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
    gl_Position = vec4(pos, 1.0);
}

#type fragment
#version 450

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

const float near = 0.01;
const float far = 100;
layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 0) out vec4 outColor;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    if (!drawAxis)
        return color;

    // z axis
    if(fragPos3D.x > -scale * minimumx && fragPos3D.x < scale * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -scale * minimumz && fragPos3D.z < scale * minimumz)
        color.x = 1.0;

    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = ubo.proj * ubo.view * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    outColor = grid(fragPos3D, 0.5, true) * float(t > 0);

    // Fading
    float rSqr = (fragPos3D.x * fragPos3D.x) + (fragPos3D.z * fragPos3D.z);
    outColor = mix(outColor, vec4(0.0), 1.0 - exp(-0.00008 * rSqr));
}