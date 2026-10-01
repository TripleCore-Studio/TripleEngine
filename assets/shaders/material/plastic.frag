#version 330 core
#extension GL_ARB_shading_language_420pack : require

in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;

layout(binding = 0) uniform sampler2D albedoMap; // textureSlots[0], slot = 0

layout(std140, binding = 0) uniform MaterialBlock {
    vec4 albedoColor;  // uniforms[0]
    float roughness;   // uniforms[1]
    float metallic;    // uniforms[2]
};

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gMaterial;

void main()
{
    vec4 albedoSample = texture(albedoMap, vUV);
    vec4 albedo = albedoSample * albedoColor;

    vec3 n = normalize(vNormal);
    vec3 packedNormal = n * 0.5 + 0.5; // [-1,1] -> [0,1] under an 8‑bit attachment

    gAlbedo = vec4(albedo.rgb, 1.0);
    gNormal = vec4(packedNormal, 1.0);
    gMaterial = vec4(metallic, roughness, 0.0, 1.0);
}
