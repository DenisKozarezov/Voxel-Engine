#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

const vec3 lightPos = vec3(2.0, 2.0, 2.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 objectColor = vec3(1.0, 1.0, 1.0);

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 result = (ambient + diffuse) * objectColor;

    outColor = vec4(result, 1.0);
}