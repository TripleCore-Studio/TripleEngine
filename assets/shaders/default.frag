#version 330 core
#extension GL_ARB_shading_language_420pack : require

in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;

layout(binding = 0) uniform sampler2D albedoMap; // textureSlots[0], slot = 0

layout(std140, binding = 0) uniform MaterialBlock {
    vec4 albedoColor;   // uniforms[0]
};

out vec4 o_FragColor;

void main()
{
    vec4 albedoSample = texture(albedoMap, vUV);
    vec4 albedo = albedoSample * albedoColor;

    o_FragColor = albedo;
}