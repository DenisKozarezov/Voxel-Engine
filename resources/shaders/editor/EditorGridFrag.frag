#version 450

const float near = 0.01;
const float far = 100;
layout(location = 2) in vec3 nearPoint;
layout(location = 3) in vec3 farPoint;
layout(location = 4) in mat4 fragViewProj;
layout(location = 0) out vec4 outColor;

vec4 computeGridColor(vec2 grid, vec2 derivative, float fragPos, float axisPos) {
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);

    if ((minimumz < -0.5 || minimumz > 0.5) || (minimumx < -0.5 || minimumx > 0.5))
        discard;

    float zAxis = (fragPos > -0.1 * minimumx && fragPos < 0.1 * minimumx) ? 1.0 : 0.1;
    float xAxis = (axisPos > -0.1 * minimumz && axisPos < 0.1 * minimumz) ? 1.0 : 0.1;
    
    return vec4(xAxis, 0.1, zAxis, 1.0 - min(line, 1.0));;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragViewProj * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    // color 1
    vec2 coord = fragPos3D.xz * 5;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    vec4 color1 = computeGridColor(grid, derivative, fragPos3D.x, fragPos3D.z);

    // color 2
    coord = fragPos3D.xz * 5;
    derivative = fwidth(coord);
    grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    vec4 color2 = computeGridColor(grid, derivative, fragPos3D.x, fragPos3D.z);

    outColor = (color1 + color2) * float(t > 0);
}