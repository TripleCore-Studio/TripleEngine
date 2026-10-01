#version 330 core
#extension GL_ARB_shading_language_420pack : require

in vec2 vUV;

layout(binding = 0) uniform sampler2D gAlbedo;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gMaterial;

layout(location = 0) out vec4 gLitColor;

void main()
{
    vec3 albedo = texture(gAlbedo, vUV).rgb;
    gLitColor = vec4(albedo, 1.0);
}
