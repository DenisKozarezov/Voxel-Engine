#type vertex
#version 450

layout(set = 0, binding = 0) readonly uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec3 lightPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outColor;
layout(location = 3) out vec3 outLightPos;
layout(location = 4) out vec3 outViewPos;

void main() {
    vec4 pos = vec4(inPosition, 1.0);

    gl_Position = ubo.proj * ubo.view * pos;

    outPosition = inPosition;
    outNormal = inNormal;
    outColor = inColor;
    outLightPos = ubo.lightPos;
    outViewPos = -pos.xyz;
}

#type fragment
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 lightPos;
layout(location = 4) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

const vec3 objectColor = vec3(1.0, 1.0, 1.0);

void main() {
    vec3 N = normalize(inNormal);
    vec3 L = normalize(lightPos);
    vec3 V = normalize(viewPos);
    vec3 R = reflect(-L, N);

    vec3 specColor = vec3(0.5);
    vec3 diffuse = objectColor * clamp(dot(L, N), 0.0, 1.0);
    vec3 ambient = objectColor * 0.1;
    outColor = vec4(diffuse + ambient, 1.0);
}