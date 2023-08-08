#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
  mat4 view;
  mat4 proj;
  mat4 viewproj;
} ubo;

void main() {
  gl_Position = ubo.viewproj * vec4(inPosition, 1.0);
  fragColor = inColor;
  fragTexCoord = inTexCoord;
}