#type vertex
#version 450 core

layout (location = 0) out vec2 outUV;

void main() {
    // Fullscreen UV
    outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(2.0 * outUV + -1.0f, 0.0f, 1.0f);
}

#type fragment
#version 450 core
#include "helper_sdf.glsl"

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) readonly uniform RaymarchData {
    vec2 resolution;
    vec2 mousePos;
    float voxelSize;
} data;

const float FOV = 1.0;
const int MAX_STEPS = 256;
const float MAX_DIST = 500;
const float EPSILON = 0.001;

vec2 fOpUnionID(vec2 res1, vec2 res2) {
    return (res1.x < res2.x) ? res1 : res2;
}

vec2 map(vec3 p) {
    float boxID = 1.0;
    float boxDist = fBoxCheap(p, vec3(data.voxelSize));
    vec2 box = vec2(boxDist, boxID);

    float planeID = 2.0;
    float planeDist = fPlane(p, vec3(0, 1, 0), 1.0);
    vec2 plane = vec2(planeDist, planeID);
    return fOpUnionID(box, plane);
}

vec2 rayMarch(vec3 ro, vec3 rd) {
    vec2 hit = vec2(0.0); vec2 object = vec2(0.0);
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + object.x * rd;
        hit = map(p);
        object.x += hit.x;
        object.y = hit.y;
        if (abs(hit.x) < EPSILON || object.x > MAX_DIST) break;
    }
    return object;
}

vec3 getNormal(vec3 p) {
    vec2 e = vec2(EPSILON, 0.0);
    vec3 n = vec3(map(p).x) - vec3(map(p - e.xyy).x, map(p - e.yxy).x, map(p - e.yyx).x);
    return normalize(n);
}

vec3 getMaterial(vec3 p, float id) {
    vec3 m;
    switch (int(id)) {
        case 1: // sphere
        m = vec3(0.9, 0.0, 0.0); break;
        case 2: // plane
        m = vec3(0.2 + 0.4 * mod(floor(p.x) + floor(p.z), 2.0)); break;
    }
    return m;
}

vec3 getLight(vec3 p, vec3 rd, vec3 color) {
    vec3 lightPos = vec3(10.0, 55.0, -20.0);
    vec3 L = normalize(lightPos - p);
    vec3 N = getNormal(p);
    vec3 V = -rd;
    vec3 R = reflect(-L, N);

    vec3 specColor = vec3(0.5);
    vec3 specular = specColor * pow(clamp(dot(R, V), 0.0, 1.0), 10.0);
    vec3 diffuse = color * clamp(dot(L, N), 0.0, 1.0);
    vec3 ambient = color * 0.05;
    vec3 fresnel = 0.25 * color * pow(1.0 + dot(rd, N), 3.0);

    // shadows
    float d = rayMarch(p + N * 0.02, normalize(lightPos)).x;
    if (d < length(lightPos - p)) return ambient + fresnel;

    return diffuse + ambient + specular + fresnel;
}

mat3 getCam(vec3 ro, vec3 lookAt) {
    vec3 camF = normalize(vec3(lookAt - ro));
    vec3 camR = normalize(cross(vec3(0, 1, 0), camF));
    vec3 camU = cross(camF, camR);
    return mat3(camR, camU, camF);
}

void mouseControl(inout vec3 ro) {
    vec2 m = data.mousePos / data.resolution;
    pR(ro.yz, m.y * PI * 0.5 - 0.5);
    pR(ro.xz, m.y * TAU);
}

void render(inout vec3 col, in vec2 uv) {
    vec3 ro = vec3(3.0, 0.0, -3.0);
    mouseControl(ro);

    vec3 lookAt = vec3(0.0, 0.0, 0.0);
    vec3 rd = getCam(ro, lookAt) * normalize(vec3(uv, FOV));

    vec2 object = rayMarch(ro, rd);
    vec3 background = vec3(0.5, 0.8, 0.9);

    if (object.x < MAX_DIST) {
        vec3 p = ro + object.x * rd;
        vec3 material = getMaterial(p, object.y);
        col += getLight(p, rd, material);
        col = mix(col, background, 1.0 - exp(-0.001 * object.x * object.x));
    } else {
        col += background - max(0.95 * rd.y, 0.0);
    }
}

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - data.resolution.xy) / data.resolution.y;
    uv.y = -uv.y; // for some reason the whole image is inverted by Y-axis...

	vec3 col = vec3(0.0);
    render(col, uv);

	fragColor = vec4(col, 1.0);
}