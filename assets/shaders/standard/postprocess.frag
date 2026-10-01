#version 330 core
#extension GL_ARB_shading_language_420pack : require

in vec2 vUV;

layout(binding = 0) uniform sampler2D gLitColor;

out vec4 o_FragColor;

void main()
{
    vec3 color = texture(gLitColor, vUV).rgb;
    o_FragColor = vec4(color, 1.0);
}
