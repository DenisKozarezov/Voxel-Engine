#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 lightPos;
layout(location = 4) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 objectColor = vec3(1.0, 1.0, 1.0);

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 normal = normalize(inNormal);
    vec3 lightDir = normalize(lightPos - inPosition);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;

    // specular
    vec3 result = (ambient + diffuse) * objectColor;

    outColor = vec4(result, 1.0);
}