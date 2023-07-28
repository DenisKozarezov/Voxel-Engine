#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 18) out;

layout (binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} MVP;

layout (location = 0) out vec3 fragColor;

void main() {
    fragColor = vec3(0.0, 1.0, 0.0);
    vec4 center = gl_in[0].gl_Position;
    float voxelSize = 0.1;

    vec4 v0 = center + vec4(-voxelSize, voxelSize, 0.0, 0.0);
    vec4 v1 = center + vec4(-voxelSize, -voxelSize, 0.0, 0.0);
    vec4 v2 = center + vec4(voxelSize, voxelSize, 0.0, 0.0);
    vec4 v3 = center + vec4(voxelSize, -voxelSize, 0.0, 0.0);
    vec4 v4 = center + vec4(voxelSize, voxelSize, -voxelSize, 0.0);
    vec4 v5 = center + vec4(voxelSize, -voxelSize, -voxelSize, 0.0);
    vec4 v6 = center + vec4(-voxelSize, voxelSize, -voxelSize, 0.0);
    vec4 v7 = center + vec4(-voxelSize, -voxelSize, -voxelSize, 0.0);

    gl_Position = MVP.viewproj * v0;
    EmitVertex();

    gl_Position = MVP.viewproj * v1;
    EmitVertex();

    gl_Position = MVP.viewproj * v2;
    EmitVertex();

    gl_Position = MVP.viewproj * v3;
    EmitVertex();

    gl_Position = MVP.viewproj * v4;
    EmitVertex();

    gl_Position = MVP.viewproj * v5;
    EmitVertex();

    gl_Position = MVP.viewproj * v6;
    EmitVertex();

    gl_Position = MVP.viewproj * v7;
    EmitVertex();

    gl_Position = MVP.viewproj * v0;
    EmitVertex();

    gl_Position = MVP.viewproj * v1;
    EmitVertex();

    EndPrimitive();
}
