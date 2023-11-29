#type vertex
#version 450

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = vec4(inPosition, 1.0);
}

#type fragment
#version 450
#include hg_sdf.glsl

const vec2 u_resolution = vec2(1920, 1080);

const float FOV = 1.0;
const int MAX_STEPS = 256;
const float MAX_DIST = 500;
const float EPS = 0.001;

vec2 map(vec3 p) {
    float sphereDist = fSphere(p, 5.0);
    return vec2(sphereDist, p.y);
}

vec2 render(inout vec3 color, in vec2 uv) {
    vec3 start = vec3(0.0, 0.0, -3.0);
    vec3 direction = normalize(vec3(uv, FOV));
}

vec2 rayMarch(vec3 start, vec3 direction) {
    vec2 hit, object;
    for (int i = 0; i < MAX_STEPS; ++i) {
        vec3 p = start + object.x * direction;
        hit = map(p);
        object.x += hit.x;
        object.y = hit.y;
        if (abs(hit.x) < EPS || object.x > MAX_DIST) break;
    }
    return object;
}

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - u_resolution.xy) / u_resolution.y;
    vec3 color;
    render(color, uv);

    // gamma correction
    col = pow(col, vec3(0.4545));
    fragColor = vec4(col, 1.0);
}