#version 450

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

layout(binding = 1) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} ubo;

layout (location = 0) in vec3 inNormal[];

layout (location = 0) out vec3 outColor;

void main() {
    float normalLength = 0.1f;
    for(int i = 0; i < gl_in.length(); i++)
	{
        vec3 pos = gl_in[i].gl_Position.xyz;
        vec3 normal = inNormal[i].xyz;

        gl_Position = ubo.proj * ubo.view * vec4(pos, 1.0);
        outColor = vec3(0.0, 1.0, 0.0);
        EmitVertex();

        gl_Position = ubo.proj * ubo.view * vec4(pos + normal * normalLength, 1.0);
        outColor = vec3(0.0, 1.0, 1.0);
        EmitVertex();

        EndPrimitive();
    }
}